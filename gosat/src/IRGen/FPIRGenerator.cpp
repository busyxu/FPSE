//===------------------------------------------------------------*- C++ -*-===//
//
// This file is distributed under MIT License. See LICENSE.txt for details.
//
//===----------------------------------------------------------------------===//
//
// Copyright (c) 2017 University of Kaiserslautern.
//

#include "FPIRGenerator.h"
#include "Utils/FPAUtils.h"
#include <llvm/ADT/SmallVector.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/CallingConv.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/MDBuilder.h>
#include <llvm/Bitcode/BitcodeWriter.h>
#include <llvm/Support/ToolOutputFile.h>
#include <iostream>


namespace gosat {

FPIRGenerator::FPIRGenerator
        (llvm::LLVMContext* context, llvm::Module* module) :
        m_has_invalid_fp_const(false),
        m_found_unsupported_smt_expr(false),
        m_gofunc(nullptr),
        m_ctx(context),
        m_mod(module)
{}

const IRSymbol* FPIRGenerator::genNumeralIR
        (llvm::IRBuilder<>& builder, const z3::expr& expr) noexcept
{
    using namespace llvm;
    if (expr.get_sort().sort_kind() == Z3_FLOATING_POINT_SORT) {
        unsigned sigd = Z3_fpa_get_sbits(expr.ctx(), expr.get_sort());
        unsigned expo = Z3_fpa_get_ebits(expr.ctx(), expr.get_sort());
        if (fpa_util::isFloat32(expo, sigd)) {
            auto result_iter = findSymbol(SymbolKind::kFP32Const, &expr);
            if (result_iter != m_expr_sym_map.cend()) {
                return &(*result_iter).second;
            }
            // TODO: handling FP32 should be configurable
            float numeral = fpa_util::toFloat32(expr);
            Value* value = ConstantFP::get(builder.getDoubleTy(), numeral);
            auto res_pair = insertSymbol(SymbolKind::kFP32Const, expr, value, 0);
            return res_pair.first;
        } else {
            if (!fpa_util::isFloat64(expo, sigd)) {
                std::cerr << "unsupported\n";
                assert(false && "unsupport expr !");
            }
            auto result_iter = findSymbol(SymbolKind::kFP64Const, &expr);
            if (result_iter != m_expr_sym_map.cend()) {
                return &(*result_iter).second;
            }
            double numeral = fpa_util::toFloat64(expr);
            Value* value = ConstantFP::get(builder.getDoubleTy(), numeral);
            auto res_pair = insertSymbol(SymbolKind::kFP64Const, expr, value, 0);
            return res_pair.first;
        }
    }

    // add by zgf to support bv const
    if (expr.get_sort().sort_kind() == Z3_BV_SORT) {
      unsigned bitWidth = Z3_get_bv_sort_size(expr.ctx(), expr.get_sort());
      if (bitWidth <= 32){
        auto result_iter = findSymbol(SymbolKind::kFP32Const, &expr);
        if (result_iter != m_expr_sym_map.cend()) {
          return &(*result_iter).second;
        }
        std::string numeral_str = Z3_ast_to_string(expr.ctx(),static_cast<z3::ast>(expr));
//        numeral_str.replace(0, 1, 1, '0');
//        int valInt = std::stoi(numeral_str);
//        float numeral = valInt;

        std::string hex_string = numeral_str.substr(2,numeral_str.size()-2);
//        char* hex_string = "3fe0000000000000";
        uint32_t int_value;
        sscanf(hex_string.c_str(), "%x", &int_value);
//        double* numeral = (double*)&int_value;

        Value* value = ConstantFP::get(builder.getDoubleTy(), int_value);
        auto res_pair = insertSymbol(SymbolKind::kFP32Const, expr, value, 0);
        return res_pair.first;
      }else{
        auto result_iter = findSymbol(SymbolKind::kFP64Const, &expr);
        if (result_iter != m_expr_sym_map.cend())
          return &(*result_iter).second;

        std::string numeral_str = Z3_ast_to_string(expr.ctx(),static_cast<z3::ast>(expr));
//        numeral_str.replace(0, 1, 1, '0');//delete "#"
//        double dval = std::stod(numeral_str);
//        long valInt = std::stoi(numeral_str,0,16);
//        double numeral = valInt;
        std::string hex_string = numeral_str.substr(2,numeral_str.size()-2);//delete "#x"
//        char* hex_string = "3fe0000000000000";
        uint64_t int_value;
        sscanf(hex_string.c_str(), "%lx", &int_value);
//        double* numeral = (double*)&int_value;
//        printf("%lf\n", *double_value);

        Value* value = ConstantFP::get(builder.getDoubleTy(), int_value);
        auto res_pair = insertSymbol(SymbolKind::kFP64Const, expr, value, 0);
        return res_pair.first;
      }
    }

    if (expr.decl().decl_kind() == Z3_OP_BNUM) {
        auto result_iter = findSymbol(SymbolKind::kFP64Const, &expr);
        if (result_iter != m_expr_sym_map.cend()) {
            return &(*result_iter).second;
        }
        std::string numeral_str = Z3_ast_to_string(expr.ctx(),
                                                   static_cast<z3::ast>(expr));
        numeral_str.replace(0, 1, 1, '0');
        Value* value = ConstantFP::get(builder.getDoubleTy(),
                                       std::stod(numeral_str));
        auto res_pair = insertSymbol(SymbolKind::kFP64Const, expr, value, 0);
        return res_pair.first;
    }
    return nullptr;
}

llvm::Function* FPIRGenerator::getDistanceFunction() const noexcept
{
    return m_func_fp64_dis;
}

llvm::Function* FPIRGenerator::genFunction
        (const z3::expr& expr)  noexcept
{
    using namespace llvm;
    if (m_gofunc != nullptr) {
        return m_gofunc;
    }
    m_gofunc = cast<Function>(
            m_mod->getOrInsertFunction(StringRef(CodeGenStr::kFunName),
                                       Type::getDoubleTy(*m_ctx),
                                       Type::getInt32Ty(*m_ctx),
                                       Type::getDoublePtrTy(*m_ctx),
                                       Type::getDoublePtrTy((*m_ctx)),
                                       Type::getInt8PtrTy(*m_ctx)));
//    llvm::outs()<<"[add by yx] m_gofunc:\n";
//    m_gofunc->print(llvm::outs());
//    llvm::outs()<<"\n";
    //declare double @gofunc(i32, double*, double*, i8*)

    Function::arg_iterator cur_arg = m_gofunc->arg_begin();

//    llvm::outs()<<"[add by yx] cur_arg:\n";
//    cur_arg->print(llvm::outs());
//    llvm::outs()<<"\n";

//  [add by yx] cur_arg:
//  i32 %0
//  double* %0
//  double* %0

    //给每个参数设置相应的属性
    (*cur_arg).setName("n");
    cur_arg++;
    (*cur_arg).setName("x");
    (*cur_arg).addAttr(Attribute::NoCapture);
    (*cur_arg).addAttr(Attribute::ReadOnly);
    cur_arg++;
    (*cur_arg).setName("grad");
    (*cur_arg).addAttr(Attribute::NoCapture);
    (*cur_arg).addAttr(Attribute::ReadNone);
    cur_arg++;
    (*cur_arg).setName("data");
    (*cur_arg).addAttr(Attribute::NoCapture);
    (*cur_arg).addAttr(Attribute::ReadNone);

//  llvm::outs()<<"[add by yx] m_gofunc2:\n";
//  m_gofunc->print(llvm::outs());
//  llvm::outs()<<"\n";
//  [add by yx] m_gofunc2:
//
//  declare double @gofunc(i32, double* nocapture readonly, double* nocapture readnone, i8* nocapture readnone)

    BasicBlock* BB = BasicBlock::Create(*m_ctx, "EntryBlock", m_gofunc);
    IRBuilder<> builder(BB);


    // TBAA Metadata
    MDBuilder md_builder(*m_ctx);
    auto md_scalar = md_builder.createConstant(builder.getInt64(0));
    auto md_node_1 = MDNode::get(*m_ctx,
                                 MDString::get(*m_ctx, "Simple C/C++ TBAA"));
    auto md_node_2 = MDNode::get(*m_ctx,
                                 {MDString::get(*m_ctx, "omnipotent char"),
                                  md_node_1, md_scalar});
    auto md_node_3 = MDNode::get(*m_ctx,
                                 {MDString::get(*m_ctx, "double"), md_node_2,
                                  md_scalar});
    m_tbaa_node = MDNode::get(*m_ctx, {md_node_3, md_node_3, md_scalar});

    // Initialize external functions to be linked to JIT
    Type *DoubleTY = Type::getDoubleTy(*m_ctx);
    Type *BoolTY = Type::getInt1Ty(*m_ctx);
    Type *Int32TY = Type::getInt32Ty(*m_ctx);

//  llvm::outs()<<"[add by yx] m_gofunc3:\n";
//  m_gofunc->print(llvm::outs());
//  llvm::outs()<<"\n";
  //  [add by yx] m_gofunc3:
//
//  define double @gofunc(i32 %n, double* nocapture readonly %x, double* nocapture readnone %grad, i8* nocapture readnone %data) {
//    EntryBlock:
//  }

    m_func_fp64_dis = cast<Function>(
            m_mod->getOrInsertFunction(StringRef(CodeGenStr::kFunDis),
                                       DoubleTY,DoubleTY,DoubleTY));

  m_func_fp64_gt_dis = cast<Function>(
          m_mod->getOrInsertFunction(StringRef(CodeGenStr::kFunGtDis),
                                     DoubleTY,DoubleTY,DoubleTY));

  m_func_fp64_lt_dis = cast<Function>(
          m_mod->getOrInsertFunction(StringRef(CodeGenStr::kFunLtDis),
                                     DoubleTY,DoubleTY,DoubleTY));

  m_func_fp64_le_dis = cast<Function>(
          m_mod->getOrInsertFunction(StringRef(CodeGenStr::kFunLeDis),
                                     DoubleTY,DoubleTY,DoubleTY));

  m_func_fp64_ge_dis = cast<Function>(
          m_mod->getOrInsertFunction(StringRef(CodeGenStr::kFunGeDis),
                                     DoubleTY,DoubleTY,DoubleTY));

  m_func_fp64_overflow_dis = cast<Function>(
          m_mod->getOrInsertFunction(StringRef(CodeGenStr::kFunOverflowDis),
                                     DoubleTY,DoubleTY,DoubleTY,DoubleTY,DoubleTY));
//    llvm::outs()<<"[add by yx]\n";
//    m_func_fp64_dis->print(llvm::outs());
//    llvm::outs()<<"\n";

    m_func_fp64_eq_dis = cast<Function>(
            m_mod->getOrInsertFunction(StringRef(CodeGenStr::kFunEqDis),
                                       DoubleTY,DoubleTY,DoubleTY));

//    llvm::outs()<<"[add by yx]\n";
//    m_func_fp64_eq_dis->print(llvm::outs());
//    llvm::outs()<<"\n";

    m_func_fp64_neq_dis = cast<Function>(
            m_mod->getOrInsertFunction(StringRef(CodeGenStr::kFunNEqDis),
                                       DoubleTY,DoubleTY,DoubleTY));
    m_func_isnan = cast<Function>(
            m_mod->getOrInsertFunction(StringRef(CodeGenStr::kFunIsNan),
                                       DoubleTY,DoubleTY,DoubleTY));
    // add by zgf
    m_func_isinf = cast<Function>(
          m_mod->getOrInsertFunction(StringRef(CodeGenStr::kFunIsInf),
                                     DoubleTY,DoubleTY,DoubleTY));
    m_func_ite = cast<Function>(
          m_mod->getOrInsertFunction(StringRef(CodeGenStr::kFunIte),
                                     DoubleTY,DoubleTY,DoubleTY,DoubleTY));
    m_func_band = cast<Function>(
          m_mod->getOrInsertFunction(StringRef(CodeGenStr::kFunBand),
                                     DoubleTY,DoubleTY,DoubleTY));
    m_func_bor = cast<Function>(
          m_mod->getOrInsertFunction(StringRef(CodeGenStr::kFunBor),
                                     DoubleTY,DoubleTY,DoubleTY));
    m_func_bxor = cast<Function>(
          m_mod->getOrInsertFunction(StringRef(CodeGenStr::kFunBxor),
                                     DoubleTY,DoubleTY,DoubleTY));

    // add by zgf : one arg
    m_func_tan = cast<Function>(
          m_mod->getOrInsertFunction(StringRef(CodeGenStr::kFunTan),
                                     DoubleTY,DoubleTY));
    m_func_asin = cast<Function>(
          m_mod->getOrInsertFunction(StringRef(CodeGenStr::kFunASin),
                                     DoubleTY,DoubleTY));
    m_func_acos = cast<Function>(
          m_mod->getOrInsertFunction(StringRef(CodeGenStr::kFunACos),
                                     DoubleTY,DoubleTY));
    m_func_atan = cast<Function>(
          m_mod->getOrInsertFunction(StringRef(CodeGenStr::kFunATan),
                                     DoubleTY,DoubleTY));
    m_func_sinh = cast<Function>(
          m_mod->getOrInsertFunction(StringRef(CodeGenStr::kFunSinh),
                                     DoubleTY,DoubleTY));
    m_func_cosh = cast<Function>(
          m_mod->getOrInsertFunction(StringRef(CodeGenStr::kFunCosh),
                                     DoubleTY,DoubleTY));
    m_func_tanh = cast<Function>(
          m_mod->getOrInsertFunction(StringRef(CodeGenStr::kFunTanh),
                                     DoubleTY,DoubleTY));
    // add by zgf : two arg
    m_func_pow = cast<Function>(
          m_mod->getOrInsertFunction(StringRef(CodeGenStr::kFunPow),
                                     DoubleTY,DoubleTY,DoubleTY));
    m_func_atan2 = cast<Function>(
          m_mod->getOrInsertFunction(StringRef(CodeGenStr::kFunATan2),
                                     DoubleTY,DoubleTY,DoubleTY));
    m_func_fmin = cast<Function>(
          m_mod->getOrInsertFunction(StringRef(CodeGenStr::kFunFMin),
                                     DoubleTY,DoubleTY,DoubleTY));
    m_func_fmax = cast<Function>(
          m_mod->getOrInsertFunction(StringRef(CodeGenStr::kFunFMax),
                                     DoubleTY,DoubleTY,DoubleTY));

    m_func_fpcheck_dis = cast<Function>(
          m_mod->getOrInsertFunction(StringRef(CodeGenStr::kFunFPDis),
                                     DoubleTY,DoubleTY,DoubleTY,Int32TY,Int32TY));
    m_func_fpcheck_fadd_overflow = cast<Function>(
          m_mod->getOrInsertFunction(StringRef(CodeGenStr::kFunFAddOverflow),
                                     BoolTY,DoubleTY,DoubleTY));
    m_func_fpcheck_fsub_overflow = cast<Function>(
          m_mod->getOrInsertFunction(StringRef(CodeGenStr::kFunFSubOverflow),
                                     BoolTY,DoubleTY,DoubleTY));
    m_func_fpcheck_fmul_overflow = cast<Function>(
          m_mod->getOrInsertFunction(StringRef(CodeGenStr::kFunFMulOverflow),
                                     BoolTY,DoubleTY,DoubleTY));
    m_func_fpcheck_fdiv_overflow = cast<Function>(
          m_mod->getOrInsertFunction(StringRef(CodeGenStr::kFunFDivOverflow),
                                     BoolTY,DoubleTY,DoubleTY));
    m_func_fpcheck_fadd_underflow = cast<Function>(
          m_mod->getOrInsertFunction(StringRef(CodeGenStr::kFunFAddUnderflow),
                                     BoolTY,DoubleTY,DoubleTY));
    m_func_fpcheck_fsub_underflow = cast<Function>(
          m_mod->getOrInsertFunction(StringRef(CodeGenStr::kFunFSubUnderflow),
                                     BoolTY,DoubleTY,DoubleTY));
    m_func_fpcheck_fmul_underflow = cast<Function>(
          m_mod->getOrInsertFunction(StringRef(CodeGenStr::kFunFMulUnderflow),
                                     BoolTY,DoubleTY,DoubleTY));
    m_func_fpcheck_fdiv_underflow = cast<Function>(
          m_mod->getOrInsertFunction(StringRef(CodeGenStr::kFunFDivUnderflow),
                                     BoolTY,DoubleTY,DoubleTY));
    m_func_fpcheck_fdiv_invalid = cast<Function>(
          m_mod->getOrInsertFunction(StringRef(CodeGenStr::kFunFDivInvalid),
                                     BoolTY,DoubleTY,DoubleTY));
    m_func_fpcheck_fdiv_zero = cast<Function>(
          m_mod->getOrInsertFunction(StringRef(CodeGenStr::kFunFDivZero),
                                     BoolTY,DoubleTY,DoubleTY));
//    add by yx
    m_func_fpcheck_finvalid_sqrt = cast<Function>(
            m_mod->getOrInsertFunction(StringRef(CodeGenStr::kFunFInvalidSqrt),
                                       BoolTY,DoubleTY,DoubleTY));
    m_func_fpcheck_finvalid_log = cast<Function>(
            m_mod->getOrInsertFunction(StringRef(CodeGenStr::kFunFInvalidLog),
                                       BoolTY,DoubleTY,DoubleTY));
    m_func_fpcheck_finvalid_pow = cast<Function>(
            m_mod->getOrInsertFunction(StringRef(CodeGenStr::kFunFInvalidPow),
                                       BoolTY,DoubleTY,DoubleTY));
  m_func_fpcheck_fadd_accuracy = cast<Function>(
          m_mod->getOrInsertFunction(StringRef(CodeGenStr::kFunFAddAccuracy),
                                     BoolTY,DoubleTY,DoubleTY));
  m_func_fpcheck_fsub_accuracy = cast<Function>(
          m_mod->getOrInsertFunction(StringRef(CodeGenStr::kFunFSubAccuracy),
                                     BoolTY,DoubleTY,DoubleTY));
  m_func_fpcheck_fmul_accuracy = cast<Function>(
          m_mod->getOrInsertFunction(StringRef(CodeGenStr::kFunFMulAccuracy),
                                     BoolTY,DoubleTY,DoubleTY));
  m_func_fpcheck_fdiv_accuracy = cast<Function>(
          m_mod->getOrInsertFunction(StringRef(CodeGenStr::kFunFDivAccuracy),
                                     BoolTY,DoubleTY,DoubleTY));
//  llvm::outs()<<"[add by yx] m_gofunc4:\n";
//  m_gofunc->print(llvm::outs());
//  llvm::outs()<<"\n";
//  [add by yx] m_gofunc4:
//
//  define double @gofunc(i32 %n, double* nocapture readonly %x, double* nocapture readnone %grad, i8* nocapture readnone %data) {
//    EntryBlock:
//  }

    m_func_fp64_dis->setLinkage(Function::ExternalLinkage);
//    [add by yx]
    m_func_fp64_gt_dis->setLinkage(Function::ExternalLinkage);
    m_func_fp64_lt_dis->setLinkage(Function::ExternalLinkage);
    m_func_fp64_ge_dis->setLinkage(Function::ExternalLinkage);
    m_func_fp64_le_dis->setLinkage(Function::ExternalLinkage);
    m_func_fp64_overflow_dis->setLinkage(Function::ExternalLinkage);
    m_func_fp64_eq_dis->setLinkage(Function::ExternalLinkage);
    m_func_fp64_neq_dis->setLinkage(Function::ExternalLinkage);
    m_func_isnan->setLinkage(Function::ExternalLinkage);
    // add by zgf
    m_func_isinf->setLinkage(Function::ExternalLinkage);
    m_func_ite->setLinkage(Function::ExternalLinkage);
    m_func_band->setLinkage(Function::ExternalLinkage);
    m_func_bor->setLinkage(Function::ExternalLinkage);
    m_func_bxor->setLinkage(Function::ExternalLinkage);
    m_func_tan->setLinkage(Function::ExternalLinkage);
    m_func_asin->setLinkage(Function::ExternalLinkage);
    m_func_acos->setLinkage(Function::ExternalLinkage);
    m_func_atan->setLinkage(Function::ExternalLinkage);
    m_func_sinh->setLinkage(Function::ExternalLinkage);
    m_func_cosh->setLinkage(Function::ExternalLinkage);
    m_func_tanh->setLinkage(Function::ExternalLinkage);
    m_func_pow->setLinkage(Function::ExternalLinkage);
    m_func_atan2->setLinkage(Function::ExternalLinkage);
    m_func_fmin->setLinkage(Function::ExternalLinkage);
    m_func_fmax->setLinkage(Function::ExternalLinkage);

    m_func_fpcheck_dis->setLinkage(Function::ExternalLinkage);
    m_func_fpcheck_fadd_overflow->setLinkage(Function::ExternalLinkage);
    m_func_fpcheck_fsub_overflow->setLinkage(Function::ExternalLinkage);
    m_func_fpcheck_fmul_overflow->setLinkage(Function::ExternalLinkage);
    m_func_fpcheck_fdiv_overflow->setLinkage(Function::ExternalLinkage);
    m_func_fpcheck_fadd_underflow->setLinkage(Function::ExternalLinkage);
    m_func_fpcheck_fsub_underflow->setLinkage(Function::ExternalLinkage);
    m_func_fpcheck_fmul_underflow->setLinkage(Function::ExternalLinkage);
    m_func_fpcheck_fdiv_underflow->setLinkage(Function::ExternalLinkage);
    m_func_fpcheck_fdiv_invalid->setLinkage(Function::ExternalLinkage);
    m_func_fpcheck_fdiv_zero->setLinkage(Function::ExternalLinkage);
//add by yx
    m_func_fpcheck_finvalid_sqrt->setLinkage(Function::ExternalLinkage);
    m_func_fpcheck_finvalid_log->setLinkage(Function::ExternalLinkage);
    m_func_fpcheck_finvalid_pow->setLinkage(Function::ExternalLinkage);
  m_func_fpcheck_fadd_accuracy->setLinkage(Function::ExternalLinkage);
  m_func_fpcheck_fsub_accuracy->setLinkage(Function::ExternalLinkage);
  m_func_fpcheck_fmul_accuracy->setLinkage(Function::ExternalLinkage);
  m_func_fpcheck_fdiv_accuracy->setLinkage(Function::ExternalLinkage);

//  llvm::outs()<<"[add by yx] m_gofunc5:\n";
//  m_gofunc->print(llvm::outs());
//  llvm::outs()<<"\n";
//
//  [add by yx] m_gofunc5:
//
//  define double @gofunc(i32 %n, double* nocapture readonly %x, double* nocapture readnone %grad, i8* nocapture readnone %data) {
//    EntryBlock:
//  }
    m_const_zero = ConstantFP::get(builder.getDoubleTy(), 0.0);
    m_const_one = ConstantFP::get(builder.getDoubleTy(), 1.0);
    //这条语句写了entryblock
    auto return_val_sym = genFuncRecursive(builder, expr, false);
//  llvm::outs()<<"[add by yx] m_gofunc7:\n";
//  m_gofunc->print(llvm::outs());
//  llvm::outs()<<"\n";
//  llvm::outs()<<*return_val_sym->getValue()<<"\n";
    builder.CreateRet(return_val_sym->getValue());

//    llvm::outs()<<"[add by yx] m_gofunc8:\n";
//    m_gofunc->print(llvm::outs());
//    llvm::outs()<<"\n";

    return m_gofunc;
}

const IRSymbol* FPIRGenerator::genFuncRecursive
        (llvm::IRBuilder<>& builder, const z3::expr expr,
         bool is_negated) noexcept
{
//    std::cerr << "[yx dbg] expr : \n"<<expr.to_string()<<"\n";
    if (!expr.is_app()) {
        // is_app <==> Z3_NUMERAL_AST || Z3_APP_AST
        std::cerr << "unsupported\n";
        assert(false && "unsupport expr !");
    }
    if (fpa_util::isRoundingModeApp(expr) &&
        expr.decl().decl_kind() != Z3_OP_FPA_RM_NEAREST_TIES_TO_EVEN &&
        expr.decl().decl_kind() != Z3_OP_FPA_RM_TOWARD_ZERO) {
        m_found_unsupported_smt_expr = true;
        assert(false && "unsupport expr !");
    }
    if (expr.is_numeral()) {
        return genNumeralIR(builder, expr);
    }
    if (fpa_util::isFPVar(expr) || fpa_util::isBVVar(expr)) {
        // TODO: handle FP16 and FP128 variables
        SymbolKind kind;
        if (fpa_util::isFloat32VarDecl(expr) ||
            fpa_util::isBV32VarDecl(expr)) {
            kind = SymbolKind::kFP32Var;
        } else if (fpa_util::isFloat64VarDecl(expr) ||
                   fpa_util::isBV64VarDecl(expr)) {
            kind = SymbolKind::kFP64Var;
        } else {
           // XXX: instead of failing directly we give it a try.
           // The result might still be useful
           m_found_unsupported_smt_expr = true;
          assert(false && "unsupport expr !");
        }
        auto result_iter = findSymbol(kind, &expr);
        if (result_iter != m_expr_sym_map.cend()) {
            return &(*result_iter).second;
        }
        using namespace llvm;
        Argument* arg2 = &(*(m_gofunc->arg_begin()+1));
//        Argument* arg2;
//        int idx = 0;
//        for (auto &arg : m_gofunc->args()){
//          if (idx == 1){
//            arg2 = &arg;
//            break;
//          }
//          idx ++;
//        }

        auto idx_ptr = builder.CreateInBoundsGEP
                (llvm::cast<Value>(arg2),
                 builder.getInt64(getVarCount()));
        auto loaded_val = builder.CreateAlignedLoad(idx_ptr, 8);
        loaded_val->setMetadata(llvm::LLVMContext::MD_tbaa, m_tbaa_node);
        auto result_pair =
                insertSymbol(kind, expr, loaded_val, getVarCount());
        m_var_sym_vec.emplace_back(result_pair.first);
        return result_pair.first;
    }
//    if (!fpa_util::isBoolExpr(expr)) {
//        is_negated = false;
//    } else if (expr.decl().decl_kind() == Z3_OP_NOT) {
//        is_negated = !is_negated;
//    }
    if (expr.decl().decl_kind() == Z3_OP_NOT) {
        is_negated = !is_negated;
    }
    SymbolKind kind = (is_negated) ? SymbolKind::kNegatedExpr : SymbolKind::kExpr;
    if (is_negated &&
        expr.decl().decl_kind() != Z3_OP_NOT &&
        expr.decl().decl_kind() != Z3_OP_AND &&
        expr.decl().decl_kind() != Z3_OP_OR) {
        // propagate negation according to de-morgan's
        is_negated = false;
    }
    auto result_iter = findSymbol(kind, &expr);
    if (result_iter != m_expr_sym_map.cend()) {
        return &(*result_iter).second;
    }
    // Expr not visited before
    std::vector<const IRSymbol*> arg_syms;
    arg_syms.reserve(expr.num_args());
    for (uint i = 0; i < expr.num_args(); ++i) {
//        llvm::outs()<<expr.arg(i).to_string()<<"\n";
        arg_syms.push_back(genFuncRecursive(builder, expr.arg(i), is_negated));
    }
    auto res_pair = insertSymbol(kind, expr, nullptr);
//    llvm::errs()<<"==================="<<arg_syms.size()<<"===================\n";
    res_pair.first->setValue(genExprIR(builder, res_pair.first, arg_syms));
//    llvm::outs()<<res_pair.first->expr()->to_string()<<"\n";
//    llvm::outs()<<*res_pair.first->getValue()<<"\n";
    if (expr.decl().decl_kind() == Z3_OP_FPA_TO_FP) {
      if (expr.num_args() == 1){
        if (fpa_util::isBVVar(expr.arg(0)))
          m_var_sym_fpa_vec.emplace_back(
                  std::make_pair(res_pair.first, arg_syms[0]));
      }else{
        if (fpa_util::isFPVar(expr.arg(1)))
          m_var_sym_fpa_vec.emplace_back(
                  std::make_pair(res_pair.first, arg_syms[1]));
      }
    }

    return res_pair.first;
}

//[yx flag]
llvm::Value* FPIRGenerator::genExprIR
        (llvm::IRBuilder<>& builder, const IRSymbol* expr_sym,
         std::vector<const IRSymbol*>& arg_syms) noexcept
{
    using namespace llvm;
    switch (expr_sym->expr()->decl().decl_kind()) {
        // Boolean operations
        case Z3_OP_TRUE:
            if (expr_sym->isNegated())
                return m_const_one;
            else
                return m_const_zero;
        case Z3_OP_FALSE:
            if (expr_sym->isNegated())
                return m_const_zero;
            else
                return m_const_one;
        case Z3_OP_EQ:
//          llvm::errs()<<"[into case Z3_OP_EQ]\n";
        case Z3_OP_FPA_EQ:
//          llvm::errs()<<"[into case Z3_OP_FPA_EQ]\n";
            return genEqualityIR(builder, expr_sym, arg_syms);
        case Z3_OP_NOT:
            // Do nothing, negation is handled with de-morgans
            return arg_syms[0]->getValue();
        case Z3_OP_AND:
            if (expr_sym->isNegated())
                return genMultiArgMulIR(builder, arg_syms);
            else
                return genMultiArgAddIR(builder, arg_syms);
        case Z3_OP_OR:
            if (expr_sym->isNegated())
                return genMultiArgAddIR(builder, arg_syms);
            else
                return genMultiArgMulIR(builder, arg_syms);
            // Floating point operations
        case Z3_OP_FPA_PLUS_INF:
            return ConstantFP::get(builder.getDoubleTy(), INFINITY);
        case Z3_OP_FPA_MINUS_INF:
            return ConstantFP::get(builder.getDoubleTy(), -INFINITY);
        case Z3_OP_FPA_NAN:
            return ConstantFP::get(builder.getDoubleTy(), NAN);
        case Z3_OP_FPA_PLUS_ZERO:
            return ConstantFP::get(builder.getDoubleTy(), 0.0);
        case Z3_OP_FPA_MINUS_ZERO:
            return ConstantFP::get(builder.getDoubleTy(), -0.0);
        case Z3_OP_BADD:
            return builder.CreateFAdd(arg_syms[0]->getValue(),
                                    arg_syms[1]->getValue());
        case Z3_OP_FPA_ADD:
            return builder.CreateFAdd(arg_syms[1]->getValue(),
                                      arg_syms[2]->getValue());

        case Z3_OP_BSUB:
            return builder.CreateFSub(arg_syms[0]->getValue(),
                                      arg_syms[1]->getValue());
        case Z3_OP_FPA_SUB:
            return builder.CreateFSub(arg_syms[1]->getValue(),
                                      arg_syms[2]->getValue());

        case Z3_OP_BNEG:
//          llvm::errs()<<"[into case Z3_OP_BNEG]\n";
        case Z3_OP_FPA_NEG:
//          llvm::errs()<<"[into case Z3_OP_FPA_NEG]\n";
            return builder.CreateFSub(
                    ConstantFP::get(builder.getDoubleTy(), -0.0),
                    arg_syms[0]->getValue());
        case Z3_OP_BMUL:
            return builder.CreateFMul(arg_syms[0]->getValue(),
                                      arg_syms[1]->getValue());
        case Z3_OP_FPA_MUL:
            return builder.CreateFMul(arg_syms[1]->getValue(),
                                      arg_syms[2]->getValue());
        case Z3_OP_BSDIV:
        case Z3_OP_BUDIV:
            return builder.CreateFDiv(arg_syms[0]->getValue(),
                                      arg_syms[1]->getValue());
        case Z3_OP_FPA_DIV:
            return builder.CreateFDiv(arg_syms[1]->getValue(),
                                      arg_syms[2]->getValue());
        case Z3_OP_BSREM:
        case Z3_OP_BUREM:
            return builder.CreateFRem(arg_syms[0]->getValue(),
                                      arg_syms[1]->getValue());
        case Z3_OP_FPA_REM:
            return builder.CreateFRem(arg_syms[1]->getValue(),
                                      arg_syms[2]->getValue());
        case Z3_OP_FPA_ABS: {
            Type *ArgType = arg_syms[0]->getValue()->getType();
            auto fabs_func = Intrinsic::getDeclaration(m_mod, Intrinsic::fabs,ArgType);
            return builder.CreateCall(fabs_func, arg_syms[0]->getValue());
        }
        case Z3_OP_FPA_SQRT: {
            // fp.sqrt roundNearestTieToEven arg1
            Type *ArgType = arg_syms[1]->getValue()->getType();
            auto sqrt_func = Intrinsic::getDeclaration(m_mod, Intrinsic::sqrt,ArgType);
            return builder.CreateCall(sqrt_func, arg_syms[1]->getValue());
        }
        case Z3_OP_SLT:
        case Z3_OP_ULT:
        case Z3_OP_LT:
//          llvm::errs()<<"[into case Z3_OP_LT]\n";
        case Z3_OP_FPA_LT:
//          llvm::outs()<<*expr_sym->getValue()<<"\n";
//          expr_sym->getValue()->print(llvm::outs());
//          llvm::outs()<<"\n";
//          llvm::outs()<<expr_sym->expr()->to_string()<<"\n";
            if (expr_sym->isNegated()) {//a<-b
                auto comp_res = builder.CreateFCmpOGE(arg_syms[0]->getValue(),
                                                      arg_syms[1]->getValue());
                return genBinArgCmpIR(builder, arg_syms, comp_res);
//                return genBinArgCmpIR3(builder, arg_syms, comp_res, Z3_OP_FPA_LT);
            }
            else {//a<b   call genBinArgCmpIR2; theta+1
              //[add by yx]  CmpLT
//              llvm::outs()<<*arg_syms[0]->getValue()<<"\n";
//              llvm::outs()<<*arg_syms[1]->getValue()<<"\n";
              auto comp_res = builder.CreateFCmpOLT(arg_syms[0]->getValue(),
                                                      arg_syms[1]->getValue());//comp_res is cmp result.
                return genBinArgCmpIR2(builder, arg_syms, comp_res);
//                return genBinArgCmpIR3(builder, arg_syms, comp_res, Z3_OP_FPA_LT);
            }
        case Z3_OP_SGT:
        case Z3_OP_UGT:
        case Z3_OP_GT:
//          llvm::errs()<<"[into case Z3_OP_GT]\n";
        case Z3_OP_FPA_GT:
            if (expr_sym->isNegated()) {//是否是一个非的表达式
                auto comp_res = builder.CreateFCmpOLE(arg_syms[0]->getValue(),
                                                      arg_syms[1]->getValue());
                return genBinArgCmpIR(builder, arg_syms, comp_res);
//                return genBinArgCmpIR3(builder, arg_syms, comp_res, Z3_OP_FPA_GT);
            } else {
                auto comp_res = builder.CreateFCmpOGT(arg_syms[0]->getValue(),
                                                      arg_syms[1]->getValue());
                return genBinArgCmpIR2(builder, arg_syms, comp_res);
//                return genBinArgCmpIR3(builder, arg_syms, comp_res, Z3_OP_FPA_GT);
            }
        case Z3_OP_SLEQ:
        case Z3_OP_ULEQ:
        case Z3_OP_LE:
        case Z3_OP_FPA_LE:
            if (expr_sym->isNegated()) {
                auto comp_res = builder.CreateFCmpOGT(arg_syms[0]->getValue(),
                                                      arg_syms[1]->getValue());
                return genBinArgCmpIR2(builder, arg_syms, comp_res);
//                return genBinArgCmpIR3(builder, arg_syms, comp_res, Z3_OP_FPA_LE);
            } else {
                auto comp_res = builder.CreateFCmpOLE(arg_syms[0]->getValue(),
                                                      arg_syms[1]->getValue());
                return genBinArgCmpIR(builder, arg_syms, comp_res);
//                return genBinArgCmpIR3(builder, arg_syms, comp_res, Z3_OP_FPA_LE);
            }
        case Z3_OP_SGEQ:
        case Z3_OP_UGEQ:
        case Z3_OP_GE:
        case Z3_OP_FPA_GE:
            if (expr_sym->isNegated()) {
                auto comp_res = builder.CreateFCmpOLT(arg_syms[0]->getValue(),
                                                      arg_syms[1]->getValue());
                return genBinArgCmpIR2(builder, arg_syms, comp_res);
//                return genBinArgCmpIR3(builder, arg_syms, comp_res, Z3_OP_FPA_GE);
            } else {
                auto comp_res = builder.CreateFCmpOGE(arg_syms[0]->getValue(),
                                                      arg_syms[1]->getValue());
                return genBinArgCmpIR(builder, arg_syms, comp_res);
//                return genBinArgCmpIR3(builder, arg_syms, comp_res, Z3_OP_FPA_GE);
            }
        case Z3_OP_SIGN_EXT:
        case Z3_OP_ZERO_EXT:
        case Z3_OP_FPA_TO_FP_UNSIGNED:
        case Z3_OP_FPA_TO_FP:{
//            return (arg_syms[arg_syms.size() - 1])->getValue();
            //            llvm::outs()<<"expr>>>"<<arg_syms[arg_syms.size()-1]->expr()->to_string()<<"\n";
            if(arg_syms[arg_syms.size()-1]->kind()!=SymbolKind::kFP32Const && arg_syms[arg_syms.size()-1]->kind()!=SymbolKind::kFP64Const){
//                llvm::outs()<<"getValue>>>"<<*arg_syms[arg_syms.size()-1]->getValue()<<"\n";
                return arg_syms[arg_syms.size()-1]->getValue();
            }
//            llvm::outs()<<arg_syms[arg_syms.size()-1]->getValue()<<"\n";
            std::string numeral_str = arg_syms[arg_syms.size()-1]->expr()->to_string();
            std::string hex_string = numeral_str.substr(2,numeral_str.size()-2);//delete "#x"
//        char* hex_string = "3fe0000000000000";
            uint64_t int_value;
            sscanf(hex_string.c_str(), "%lx", &int_value);
//            double *numeral = (double*)&int_value;
            double numeral = 0.0;
            if(arg_syms[arg_syms.size()-1]->kind()==SymbolKind::kFP32Const){
                float *numeral32 = (float *)&int_value;
                numeral = *numeral32;
            }
            else if(arg_syms[arg_syms.size()-1]->kind()==SymbolKind::kFP64Const){
                double *numeral64 = (double*)&int_value;
                numeral = *numeral64;
            }
            else{
                assert(true && "unsupport SymbolKind!");
            }
//            llvm::outs()<<"numeral>>>"<<numeral<<"\n";
            Value* value = ConstantFP::get(builder.getDoubleTy(),numeral);
            return value;
        }

        case Z3_OP_FPA_IS_NAN:
            if (expr_sym->isNegated()) {
                auto call_res = builder.CreateCall(m_func_isnan, {arg_syms[0]->getValue(), m_const_one});
                call_res->setTailCall(false);
                return call_res;
            } else {
                auto call_res = builder.CreateCall(m_func_isnan, {arg_syms[0]->getValue(), m_const_zero});
                call_res->setTailCall(false);
                return call_res;
            }

        // add by zgf : adopt the same operation to Z3_OP_FPA_TO_FP
        case Z3_OP_CONCAT:{
          std::string highBit64 = arg_syms[0]->expr()->to_string();
          bool APFLosesInfo;
          if (highBit64.find("43feffff") != std::string::npos){
            APFloat DBLmax = APFloat::getLargest(
                    APFloat::IEEEdouble(), false);
            DBLmax.convert(APFloat::IEEEquad(),
                           APFloat::rmNearestTiesToEven,
                           &APFLosesInfo);
            ConstantFP::get(*m_ctx,DBLmax);
          }else{
            APFloat DBLmin = APFloat::getSmallest(
                    APFloat::IEEEdouble(), false);
            DBLmin.convert(APFloat::IEEEquad(),
                           APFloat::rmNearestTiesToEven,
                           &APFLosesInfo);
            ConstantFP::get(*m_ctx,DBLmin);
          }
        }

        case Z3_OP_FPA_IS_INF:
            if (expr_sym->isNegated()) {
              auto call_res = builder.CreateCall(m_func_isinf, {arg_syms[0]->getValue(), m_const_one});
              call_res->setTailCall(false);
              return call_res;
            } else {
              auto call_res = builder.CreateCall(m_func_isinf, {arg_syms[0]->getValue(), m_const_zero});
              call_res->setTailCall(false);
              return call_res;
            }
        case Z3_OP_ITE:
            return builder.CreateCall(m_func_ite, {arg_syms[0]->getValue(),
                                                   arg_syms[1]->getValue(),
                                                   arg_syms[2]->getValue()});
        case Z3_OP_BAND:
            return builder.CreateCall(m_func_band, {arg_syms[0]->getValue(),
                                                    arg_syms[1]->getValue()});
        case Z3_OP_FPA_TO_IEEE_BV:
        case Z3_OP_FPA_TO_UBV:
        case Z3_OP_FPA_TO_SBV:
            return (arg_syms[arg_syms.size() - 1])->getValue();
        case Z3_OP_UNINTERPRETED:{
          std::string funcName = expr_sym->expr()->decl().name().str();
          Type *ArgType = arg_syms[0]->getValue()->getType(); // type must all be fp64
          llvm::Function* FPFunc;
          if (expr_sym->expr()->num_args() == 1){
            if (funcName == "CF_log")
              FPFunc = Intrinsic::getDeclaration(m_mod, Intrinsic::log,ArgType);
            else if (funcName == "CF_exp")
              FPFunc = Intrinsic::getDeclaration(m_mod, Intrinsic::exp,ArgType);
            else if (funcName == "CF_floor")
              FPFunc = Intrinsic::getDeclaration(m_mod, Intrinsic::floor,ArgType);
            else if (funcName == "CF_ceil")
              FPFunc = Intrinsic::getDeclaration(m_mod, Intrinsic::ceil,ArgType);
            else if (funcName == "CF_sin")
              FPFunc = Intrinsic::getDeclaration(m_mod, Intrinsic::sin,ArgType);
            else if (funcName == "CF_cos")
              FPFunc = Intrinsic::getDeclaration(m_mod, Intrinsic::cos,ArgType);
            else if (funcName == "CF_tan")
              FPFunc = m_func_tan;
            else if (funcName == "CF_asin")
              FPFunc = m_func_asin;
            else if (funcName == "CF_acos")
              FPFunc = m_func_acos;
            else if (funcName == "CF_atan")
              FPFunc = m_func_atan;
            else if (funcName == "CF_sinh")
              FPFunc = m_func_sinh;
            else if (funcName == "CF_cosh")
              FPFunc = m_func_cosh;
            else if (funcName == "CF_tanh")
              FPFunc = m_func_tanh;

            return builder.CreateCall(FPFunc, arg_syms[0]->getValue());
          }
          else{
            bool isCommonFunc = false;
            int opcode = 0, mode = 0;
            if (funcName == "CF_pow"){
              FPFunc = m_func_pow;
              isCommonFunc = true;
            }else if (funcName == "CF_atan2"){
              FPFunc = m_func_atan2;
              isCommonFunc = true;
            }else if (funcName == "CF_fmin"){
              FPFunc = m_func_fmin;
              isCommonFunc = true;
            }else if (funcName == "CF_fmax"){
              FPFunc = m_func_fmax;
              isCommonFunc = true;
            }
            else if (funcName == "FPCHECK_FADD_OVERFLOW"){
              FPFunc = m_func_fpcheck_fadd_overflow;
              opcode = 1; mode = 1;
            }
            else if (funcName == "FPCHECK_FSUB_OVERFLOW"){
              FPFunc = m_func_fpcheck_fsub_overflow;
              opcode = 2; mode = 1;
            }
            else if (funcName == "FPCHECK_FMUL_OVERFLOW"){
              FPFunc = m_func_fpcheck_fmul_overflow;
              opcode = 3; mode = 1;
            }
            else if (funcName == "FPCHECK_FDIV_OVERFLOW"){
              FPFunc = m_func_fpcheck_fdiv_overflow;
              opcode = 4; mode = 1;
            }
            else if (funcName == "FPCHECK_FADD_UNDERFLOW"){
              FPFunc = m_func_fpcheck_fadd_underflow;
              opcode = 1; mode = 2;
            }
            else if (funcName == "FPCHECK_FSUB_UNDERFLOW"){
              FPFunc = m_func_fpcheck_fsub_underflow;
              opcode = 2; mode = 2;
            }
            else if (funcName == "FPCHECK_FMUL_UNDERFLOW"){
              FPFunc = m_func_fpcheck_fmul_underflow;
              opcode = 3; mode = 2;
            }
            else if (funcName == "FPCHECK_FDIV_UNDERFLOW"){
              FPFunc = m_func_fpcheck_fdiv_underflow;
              opcode = 4; mode = 2;
            }
            else if (funcName == "FPCHECK_FDIV_INVALID"){
              FPFunc = m_func_fpcheck_fdiv_invalid;
              opcode = 4; mode = 3;
            }
            else if (funcName == "FPCHECK_FDIV_ZERO"){
              FPFunc = m_func_fpcheck_fdiv_zero;
              opcode = 4; mode = 4;
            }
            else if (funcName == "FPCHECK_FINVALID_SQRT"){//add by yx
              FPFunc = m_func_fpcheck_finvalid_sqrt;
              opcode = 5; mode = 1;
            }
            else if (funcName == "FPCHECK_FINVALID_LOG"){//add by yx
              FPFunc = m_func_fpcheck_finvalid_log;
              opcode = 5; mode = 2;
            }
            else if (funcName == "FPCHECK_FINVALID_POW"){//add by yx
              FPFunc = m_func_fpcheck_finvalid_pow;
              opcode = 5; mode = 3;
            }
            else if (funcName == "FPCHECK_FADD_ACCURACY"){
              FPFunc = m_func_fpcheck_fadd_accuracy;
              opcode = 1; mode = 1;
            }
            else if (funcName == "FPCHECK_FSUB_ACCURACY"){
              FPFunc = m_func_fpcheck_fsub_accuracy;
              opcode = 2; mode = 1;
            }
            else if (funcName == "FPCHECK_FMUL_ACCURACY"){
              FPFunc = m_func_fpcheck_fmul_accuracy;
              opcode = 3; mode = 1;
            }
            else if (funcName == "FPCHECK_FDIV_ACCURACY"){
              FPFunc = m_func_fpcheck_fdiv_accuracy;
              opcode = 4; mode = 1;
            }
            else{
              std::cerr << "unsupported: " +
                           expr_sym->expr()->to_string() + "\n";
              assert(false && "uninterpreter function is not support now");
            }

            auto call_res = builder.CreateCall(FPFunc,{arg_syms[0]->getValue(),
                                                       arg_syms[1]->getValue()});

            if (isCommonFunc){
              call_res->setTailCall(false);
              return call_res;
            }else{
              return genFPCheckIR(builder, arg_syms, call_res, opcode ,mode);
            }
          }
        }

        default:
            std::cerr << "unsupported: " +
                         expr_sym->expr()->decl().name().str() + "\n";
            std::exit(3);
    }
}

llvm::Value* FPIRGenerator::genBinArgCmpIR
        (llvm::IRBuilder<>& builder, std::vector<const IRSymbol*>& arg_syms,
         llvm::Value* comp_result) noexcept
{
    using namespace llvm;
    BasicBlock* bb_first = BasicBlock::Create(*m_ctx, "", m_gofunc);
    BasicBlock* bb_second = BasicBlock::Create(*m_ctx, "", m_gofunc);
    BasicBlock* bb_cur = builder.GetInsertBlock();
    builder.CreateCondBr(comp_result, bb_second, bb_first);
    builder.SetInsertPoint(bb_first);
    auto call_res = builder.CreateCall(m_func_fp64_dis, {arg_syms[0]->getValue(),
                                                    arg_syms[1]->getValue()});
    call_res->setTailCall(false);
    builder.CreateBr(bb_second);
    builder.SetInsertPoint(bb_second);
    auto phi_inst = builder.CreatePHI(builder.getDoubleTy(), 2);
    phi_inst->addIncoming(call_res, bb_first);//要么是这个距离 call_res
    phi_inst->addIncoming(m_const_zero, bb_cur);//要么是0
    return phi_inst;
}

//[add by yx] cmpIR3
//llvm::Value* FPIRGenerator::genBinArgCmpIR3
//        (llvm::IRBuilder<>& builder, std::vector<const IRSymbol*>& arg_syms,
//         llvm::Value* comp_result, Z3_decl_kind kind) noexcept
//{
//  using namespace llvm;
//  BasicBlock* bb_first = BasicBlock::Create(*m_ctx, "", m_gofunc);
//  BasicBlock* bb_second = BasicBlock::Create(*m_ctx, "", m_gofunc);
//  BasicBlock* bb_cur = builder.GetInsertBlock();
////  llvm::outs()<<"comp_result:\n"<<*comp_result<<"\n";
//  builder.CreateCondBr(comp_result, bb_second, bb_first);//comp_result is jude cond if success
//  builder.SetInsertPoint(bb_first);
//  llvm::Function* disfunc = m_func_fp64_dis;
//  switch (kind) {
//    case Z3_OP_FPA_LT:
//      disfunc = m_func_fp64_lt_dis;
//      break;
//    case Z3_OP_FPA_GT:
//      disfunc = m_func_fp64_gt_dis;
//      break;
//    case Z3_OP_FPA_LE:
//      disfunc = m_func_fp64_le_dis;
//      break;
//    case Z3_OP_FPA_GE:
//      disfunc = m_func_fp64_ge_dis;
//      break;
//    default:
//      break;
//  }
//  auto call_res = builder.CreateCall(disfunc, {arg_syms[0]->getValue(), arg_syms[1]->getValue()});
//  call_res->setTailCall(false);
//  builder.CreateBr(bb_second);
//  builder.SetInsertPoint(bb_second);
//  auto phi_inst = builder.CreatePHI(builder.getDoubleTy(), 2);
//  phi_inst->addIncoming(call_res, bb_first);//要么是这个距离 call_res
//  phi_inst->addIncoming(m_const_zero, bb_cur);//要么是0
////  llvm::errs()<<*phi_inst<<"\n";
//  return phi_inst;
//}


llvm::Value* FPIRGenerator::genFPCheckIR(llvm::IRBuilder<> &builder,
                                         std::vector<const IRSymbol*>& arg_syms,
                                         llvm::Value* comp_result,
                                         int opcode,
                                         int mode) noexcept {
  using namespace llvm;
  BasicBlock* bb_first = BasicBlock::Create(*m_ctx, "", m_gofunc);
  BasicBlock* bb_second = BasicBlock::Create(*m_ctx, "", m_gofunc);
  BasicBlock* bb_cur = builder.GetInsertBlock();
  builder.CreateCondBr(comp_result, bb_second, bb_first);
  builder.SetInsertPoint(bb_first);
  Value* valueOpcode = ConstantInt::get(builder.getInt32Ty(),opcode);
  Value* valueMode = ConstantInt::get(builder.getInt32Ty(),mode);
  auto call_res = builder.CreateCall(m_func_fpcheck_dis, {arg_syms[0]->getValue(),
                                                               arg_syms[1]->getValue(),
                                                               valueOpcode,
                                                               valueMode});
  call_res->setTailCall(false);
  builder.CreateBr(bb_second);
  builder.SetInsertPoint(bb_second);
  auto phi_inst = builder.CreatePHI(builder.getDoubleTy(), 2);
  phi_inst->addIncoming(call_res, bb_first);
  phi_inst->addIncoming(m_const_zero, bb_cur);
  return phi_inst;
}

llvm::Value* FPIRGenerator::genBinArgCmpIR2
        (llvm::IRBuilder<>& builder, std::vector<const IRSymbol*>& arg_syms,
         llvm::Value* comp_result) noexcept
{
    using namespace llvm;
    BasicBlock* bb_first = BasicBlock::Create(*m_ctx, "", m_gofunc);
    BasicBlock* bb_second = BasicBlock::Create(*m_ctx, "", m_gofunc);
    BasicBlock* bb_cur = builder.GetInsertBlock();
    builder.CreateCondBr(comp_result, bb_second, bb_first);
    builder.SetInsertPoint(bb_first);
    auto call_res = builder.CreateCall(m_func_fp64_dis, {arg_syms[0]->getValue(),
                                                    arg_syms[1]->getValue()});
    call_res->setTailCall(false);
    auto dis_res = builder.CreateFAdd(call_res, m_const_one);//when <:      theta(e1,e2)+1
    builder.CreateBr(bb_second);
    builder.SetInsertPoint(bb_second);
    auto phi_inst = builder.CreatePHI(builder.getDoubleTy(), 2);
    phi_inst->addIncoming(dis_res, bb_first);
    phi_inst->addIncoming(m_const_zero, bb_cur);
    return phi_inst;
}

llvm::Value* FPIRGenerator::genMultiArgAddIR
        (llvm::IRBuilder<>& builder,
         std::vector<const IRSymbol*>& arg_syms) noexcept
{
    auto result = builder.CreateFAdd(arg_syms[0]->getValue(),
                                     arg_syms[1]->getValue());
    for (unsigned i = 2; i < arg_syms.size(); ++i) {
        result = builder.CreateFAdd(result, arg_syms[i]->getValue());
    }
    return result;
}

llvm::Value* FPIRGenerator::genMultiArgMulIR
        (llvm::IRBuilder<>& builder,
         std::vector<const IRSymbol*>& arg_syms) noexcept
{
    auto result = builder.CreateFMul(arg_syms[0]->getValue(),
                                     arg_syms[1]->getValue());
    for (unsigned i = 2; i < arg_syms.size(); ++i) {
        result = builder.CreateFMul(result, arg_syms[i]->getValue());
    }
    return result;
}

llvm::Value *FPIRGenerator::genEqualityIR
        (llvm::IRBuilder<> &builder, const IRSymbol *expr_sym,
         std::vector<const IRSymbol *> &arg_syms) noexcept
{

    // workaround were we explicitly check the sort of the arguments,
    // because z3 provides Z3_OP_EQ were we expect Z3_OP_FPA_EQ.
    // See wintersteiger/eq/eq-has-no-other-solution-10015.smt2

    bool is_fpa_args = (arg_syms[0]->expr()->get_sort().sort_kind() ==
                        Z3_FLOATING_POINT_SORT);
    assert(is_fpa_args == (arg_syms[1]->expr()->get_sort().sort_kind() ==
                           Z3_FLOATING_POINT_SORT));
    if (expr_sym->expr()->decl().decl_kind() == Z3_OP_FPA_EQ || is_fpa_args) {
        if (expr_sym->isNegated()) {
            auto result = builder.CreateFCmpONE(arg_syms[0]->getValue(),
                                                arg_syms[1]->getValue());
            return builder.CreateSelect(result, m_const_zero, m_const_one);
        } else {
            return builder.CreateCall(m_func_fp64_eq_dis, {arg_syms[0]->getValue(),
                                                        arg_syms[1]->getValue()});
        }
    }
    // assuming Z3_OP_EQ
    if (expr_sym->isNegated()) {
        return builder.CreateCall(m_func_fp64_neq_dis,
                                  {arg_syms[0]->getValue(),
                                   arg_syms[1]->getValue()});
    } else {
        return builder.CreateCall(m_func_fp64_eq_dis,
                                  {arg_syms[0]->getValue(),
                                   arg_syms[1]->getValue()});
    }
}

unsigned FPIRGenerator::getVarCount() const noexcept
{
    return static_cast<unsigned>(m_var_sym_vec.size());
}

const std::vector<IRSymbol*>&
FPIRGenerator::getVars() const noexcept
{
    return m_var_sym_vec;
}

const std::vector<std::pair<IRSymbol*, const IRSymbol*>>&
FPIRGenerator::getVarsFPAWrapped() const noexcept
{
    return m_var_sym_fpa_vec;
}

std::pair<IRSymbol*, bool> FPIRGenerator::insertSymbol
        (const SymbolKind kind, const z3::expr expr, llvm::Value* value,
         unsigned id) noexcept
{
    if (kind != SymbolKind::kNegatedExpr) {
        auto result = m_expr_sym_map.insert
                ({expr.hash(), IRSymbol(kind, expr, value, id)});
        return {&(*result.first).second, result.second};
    } else {
        // XXX: would this weaken hashing?
        auto result = m_expr_sym_map.insert
                ({expr.hash() + static_cast<unsigned>(kind),
                  IRSymbol(kind, expr, value, id)});
        return {&(*result.first).second, result.second};
    }
}

SymMapType::const_iterator FPIRGenerator::findSymbol
        (const SymbolKind kind, const z3::expr* expr) const noexcept
{
    if (kind != SymbolKind::kNegatedExpr) {
        return m_expr_sym_map.find(expr->hash());
    } else {
        // XXX: would this weaken hashing?
        return m_expr_sym_map.find(expr->hash() + static_cast<unsigned>(kind));
    }
}

void FPIRGenerator::addGlobalFunctionMappings(llvm::ExecutionEngine *engine)
{
    double (*func_ptr_dis)(double, double) = fp64_dis;

    //add by yx
    double (*func_ptr_gt_dis)(double, double) = fp64_gt_dis;
    double (*func_ptr_lt_dis)(double, double) = fp64_lt_dis;
    double (*func_ptr_ge_dis)(double, double) = fp64_ge_dis;
    double (*func_ptr_le_dis)(double, double) = fp64_le_dis;
    double (*func_ptr_overflow_dis)(double, double, int, int) = fp64_overflow_dis;

    double (*func_ptr_eq_dis)(double, double) = fp64_eq_dis;
    double (*func_ptr_neq_dis)(double, double) = fp64_neq_dis;
    double (*func_ptr_isnan)(double, double) = fp64_isnan;
    // add by zgf
    double (*func_ptr_isinf)(double, double) = fp64_isinf;
    double (*func_ptr_ite)(double, double, double) = fp64_ite;
    double (*func_ptr_band)(double, double) = fp64_band;
    double (*func_ptr_bor)(double, double) = fp64_bor;
    double (*func_ptr_bxor)(double, double) = fp64_bxor;
    double (*func_ptr_tan)(double) = fp64_tan;
    double (*func_ptr_asin)(double) = fp64_asin;
    double (*func_ptr_acos)(double) = fp64_acos;
    double (*func_ptr_atan)(double) = fp64_atan;
    double (*func_ptr_sinh)(double) = fp64_sinh;
    double (*func_ptr_cosh)(double) = fp64_cosh;
    double (*func_ptr_tanh)(double) = fp64_tanh;
    double (*func_ptr_pow)(double, double) = fp64_pow;
    double (*func_ptr_atan2)(double, double) = fp64_atan2;
    double (*func_ptr_fmin)(double, double) = fp64_fmin;
    double (*func_ptr_fmax)(double, double) = fp64_fmax;

    double (*func_ptr_fpcheck_dis)(double, double,int, int) = fpcheck_dis;
    bool (*func_ptr_fpcheck_fadd_overflow)(double, double) = FPCHECK_FADD_OVERFLOW;
    bool (*func_ptr_fpcheck_fsub_overflow)(double, double) = FPCHECK_FSUB_OVERFLOW;
    bool (*func_ptr_fpcheck_fmul_overflow)(double, double) = FPCHECK_FMUL_OVERFLOW;
    bool (*func_ptr_fpcheck_fdiv_overflow)(double, double) = FPCHECK_FDIV_OVERFLOW;
    bool (*func_ptr_fpcheck_fadd_underflow)(double, double) = FPCHECK_FADD_UNDERFLOW;
    bool (*func_ptr_fpcheck_fsub_underflow)(double, double) = FPCHECK_FSUB_UNDERFLOW;
    bool (*func_ptr_fpcheck_fmul_underflow)(double, double) = FPCHECK_FMUL_UNDERFLOW;
    bool (*func_ptr_fpcheck_fdiv_underflow)(double, double) = FPCHECK_FDIV_UNDERFLOW;
    bool (*func_ptr_fpcheck_fdiv_invalid)(double, double) = FPCHECK_FDIV_INVALID;
    bool (*func_ptr_fpcheck_fdiv_zero)(double, double) = FPCHECK_FDIV_ZERO;
//add by yx
    bool (*func_ptr_fpcheck_finvalid_sqrt)(double, double) = FPCHECK_FINVALID_SQRT;
    bool (*func_ptr_fpcheck_finvalid_log)(double, double) = FPCHECK_FINVALID_LOG;
    bool (*func_ptr_fpcheck_finvalid_pow)(double, double) = FPCHECK_FINVALID_POW;

    bool (*func_ptr_fpcheck_fadd_accuracy)(double, double) = FPCHECK_FADD_ACCURACY;
    bool (*func_ptr_fpcheck_fsub_accuracy)(double, double) = FPCHECK_FSUB_ACCURACY;
    bool (*func_ptr_fpcheck_fmul_accuracy)(double, double) = FPCHECK_FMUL_ACCURACY;
    bool (*func_ptr_fpcheck_fdiv_accuracy)(double, double) = FPCHECK_FDIV_ACCURACY;

    engine->addGlobalMapping(this->m_func_fp64_dis, (void *)func_ptr_dis);
//    [add by yx]
    engine->addGlobalMapping(this->m_func_fp64_gt_dis, (void *)func_ptr_gt_dis);
    engine->addGlobalMapping(this->m_func_fp64_lt_dis, (void *)func_ptr_lt_dis);
    engine->addGlobalMapping(this->m_func_fp64_ge_dis, (void *)func_ptr_gt_dis);
    engine->addGlobalMapping(this->m_func_fp64_le_dis, (void *)func_ptr_lt_dis);
    engine->addGlobalMapping(this->m_func_fp64_overflow_dis, (void *)func_ptr_lt_dis);

    engine->addGlobalMapping(this->m_func_fp64_eq_dis, (void *)func_ptr_eq_dis);
    engine->addGlobalMapping(this->m_func_fp64_neq_dis, (void *)func_ptr_neq_dis);
    engine->addGlobalMapping(this->m_func_isnan, (void *)func_ptr_isnan);
    // add by zgf
    engine->addGlobalMapping(this->m_func_isinf, (void *)func_ptr_isinf);
    engine->addGlobalMapping(this->m_func_ite, (void *)func_ptr_ite);
    engine->addGlobalMapping(this->m_func_band, (void *)func_ptr_band);
    engine->addGlobalMapping(this->m_func_bor, (void *)func_ptr_bor);
    engine->addGlobalMapping(this->m_func_bxor, (void *)func_ptr_bxor);
    engine->addGlobalMapping(this->m_func_tan, (void *)func_ptr_tan);
    engine->addGlobalMapping(this->m_func_asin, (void *)func_ptr_asin);
    engine->addGlobalMapping(this->m_func_acos, (void *)func_ptr_acos);
    engine->addGlobalMapping(this->m_func_atan, (void *)func_ptr_atan);
    engine->addGlobalMapping(this->m_func_sinh, (void *)func_ptr_sinh);
    engine->addGlobalMapping(this->m_func_cosh, (void *)func_ptr_cosh);
    engine->addGlobalMapping(this->m_func_tanh, (void *)func_ptr_tanh);
    engine->addGlobalMapping(this->m_func_pow, (void *)func_ptr_pow);
    engine->addGlobalMapping(this->m_func_atan2, (void *)func_ptr_atan2);
    engine->addGlobalMapping(this->m_func_fmin, (void *)func_ptr_fmin);
    engine->addGlobalMapping(this->m_func_fmax, (void *)func_ptr_fmax);

    engine->addGlobalMapping(this->m_func_fpcheck_dis,
                           (void *)func_ptr_fpcheck_dis);
    engine->addGlobalMapping(this->m_func_fpcheck_fadd_overflow,
                            (void *)func_ptr_fpcheck_fadd_overflow);
    engine->addGlobalMapping(this->m_func_fpcheck_fsub_overflow,
                            (void *)func_ptr_fpcheck_fsub_overflow);
    engine->addGlobalMapping(this->m_func_fpcheck_fmul_overflow,
                            (void *)func_ptr_fpcheck_fmul_overflow);
    engine->addGlobalMapping(this->m_func_fpcheck_fdiv_overflow,
                            (void *)func_ptr_fpcheck_fdiv_overflow);
    engine->addGlobalMapping(this->m_func_fpcheck_fadd_underflow,
                            (void *)func_ptr_fpcheck_fadd_underflow);
    engine->addGlobalMapping(this->m_func_fpcheck_fsub_underflow,
                           (void *)func_ptr_fpcheck_fsub_underflow);
    engine->addGlobalMapping(this->m_func_fpcheck_fmul_underflow,
                           (void *)func_ptr_fpcheck_fmul_underflow);
    engine->addGlobalMapping(this->m_func_fpcheck_fdiv_underflow,
                           (void *)func_ptr_fpcheck_fdiv_underflow);
    engine->addGlobalMapping(this->m_func_fpcheck_fdiv_invalid,
                           (void *)func_ptr_fpcheck_fdiv_invalid);
    engine->addGlobalMapping(this->m_func_fpcheck_fdiv_zero,
                           (void *)func_ptr_fpcheck_fdiv_zero);
//ADD BY YX
    engine->addGlobalMapping(this->m_func_fpcheck_finvalid_sqrt,
                             (void *)func_ptr_fpcheck_finvalid_sqrt);
    engine->addGlobalMapping(this->m_func_fpcheck_finvalid_log,
                             (void *)func_ptr_fpcheck_finvalid_log);
    engine->addGlobalMapping(this->m_func_fpcheck_finvalid_pow,
                             (void *)func_ptr_fpcheck_finvalid_pow);
    engine->addGlobalMapping(this->m_func_fpcheck_fadd_accuracy,
                             (void *)func_ptr_fpcheck_fadd_accuracy);
    engine->addGlobalMapping(this->m_func_fpcheck_fsub_accuracy,
                             (void *)func_ptr_fpcheck_fsub_accuracy);
    engine->addGlobalMapping(this->m_func_fpcheck_fmul_accuracy,
                             (void *)func_ptr_fpcheck_fmul_accuracy);
    engine->addGlobalMapping(this->m_func_fpcheck_fdiv_accuracy,
                             (void *)func_ptr_fpcheck_fdiv_accuracy);
}

bool FPIRGenerator::isFoundUnsupportedSMTExpr() noexcept
{
    return m_found_unsupported_smt_expr;
}
}
