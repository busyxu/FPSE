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
#include <regex>

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
        (llvm::IRBuilder<>& builder, const z3::expr& expr, std::vector<double>& init_number) noexcept
{
//    llvm::outs()<<"numermal expr>>>"<<expr.to_string()<<"\n";
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
            init_number.push_back(numeral);// add by yx
            Value* value = ConstantFP::get(builder.getDoubleTy(), numeral);// to_double, to type consistency
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
            init_number.push_back(numeral);// add by yx
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

        std::string hex_string = numeral_str.substr(2,numeral_str.size()-2);
//        char* hex_string = "3fe0000000000000";
        uint32_t int_value;
        sscanf(hex_string.c_str(), "%x", &int_value);
//        double* numeral = (double*)&int_value;
////        printf("numeral: %lf\n", *numeral);
//        init_number.push_back(*numeral);
//        Value* value = ConstantFP::get(builder.getDoubleTy(),*numeral);
//        auto res_pair = insertSymbol(SymbolKind::kFP32Const, expr, value, 0);
//          init_number.push_back(int_value);
        Value* value = ConstantFP::get(builder.getDoubleTy(), int_value);// to_double, to type consistency
        auto res_pair = insertSymbol(SymbolKind::kFP32Const, expr, value, 0);
//          llvm::outs()<<*res_pair.first->getValue()<<"\n";
        return res_pair.first;
      }else{
        auto result_iter = findSymbol(SymbolKind::kFP64Const, &expr);
        if (result_iter != m_expr_sym_map.cend())
          return &(*result_iter).second;

        std::string numeral_str = Z3_ast_to_string(expr.ctx(),static_cast<z3::ast>(expr));
        std::string hex_string = numeral_str.substr(2,numeral_str.size()-2);//delete "#x"
//        char* hex_string = "3fe0000000000000";
        uint64_t int_value;
        sscanf(hex_string.c_str(), "%lx", &int_value);
//        double* numeral = (double*)&int_value;
////        printf("numeral: int>%ld; double>%lf\n",int_value, *numeral);
//        init_number.push_back(*numeral);
//        Value* value = ConstantFP::get(builder.getDoubleTy(),*numeral);
//        auto res_pair = insertSymbol(SymbolKind::kFP64Const, expr, value, 0);
//          init_number.push_back(int_value);
          Value* value = ConstantFP::get(builder.getDoubleTy(), int_value);
          auto res_pair = insertSymbol(SymbolKind::kFP64Const, expr, value, 0);
//          llvm::outs()<<*res_pair.first->getValue()<<"\n";
        return res_pair.first;
      }
    }

//    // add by yx support real const
//    if (expr.get_sort().sort_kind() == Z3_REAL_SORT) {
//        auto result_iter = findSymbol(SymbolKind::kFP64Const, &expr);
//        if (result_iter != m_expr_sym_map.cend())
//            return &(*result_iter).second;
//
//        std::string numeral_str = Z3_ast_to_string(expr.ctx(),static_cast<z3::ast>(expr));
//        double numeral = std::stod(numeral_str);
//        init_number.push_back(numeral);
////        printf("numeral: str>%s; double>%lf\n",numeral_str.c_str(), numeral);
//
//        Value* value = ConstantFP::get(builder.getDoubleTy(),numeral);
//        auto res_pair = insertSymbol(SymbolKind::kFP64Const, expr, value, 0);
//        return res_pair.first;
//    }

//    add by yx support real expr
    if (expr.get_sort().sort_kind() == Z3_REAL_SORT) {
        auto result_iter = findSymbol(SymbolKind::kFP64Const, &expr);
        if (result_iter != m_expr_sym_map.cend()) {
            return &(*result_iter).second;
        }
        std::string numeral_str = Z3_ast_to_string(expr.ctx(),
                                                   static_cast<z3::ast>(expr));
        std::regex pattern(R"(^\(.*\)$)");
        if(std::regex_match(numeral_str, pattern)){
            numeral_str = numeral_str.substr(1, numeral_str.size() - 2);
            std::vector<std::string> tokens;
            std::istringstream stream(numeral_str);
            std::string token;
            char delimiter = ' ';
            while (std::getline(stream, token, delimiter)) {
                tokens.push_back(token);
            }
            char* op = const_cast<char *>(tokens[0].c_str());
            double resDouble = 0;
            switch (*op) {
                case '+':{
                    double op1 = std::stod(tokens[1]);
                    double op2 = std::stod(tokens[2]);
                    resDouble = op1 + op2;
                    break;
                }
                case '-':{
                    double op1 = std::stod(tokens[1]);
                    double op2 = std::stod(tokens[2]);
                    resDouble = op1 - op2;
                    break;
                }
                case '*':{
                    double op1 = std::stod(tokens[1]);
                    double op2 = std::stod(tokens[2]);
                    resDouble = op1 * op2;
                    break;
                }
                case '/':{
                    double op1 = std::stod(tokens[1]);
                    double op2 = std::stod(tokens[2]);
                    resDouble = op1 / op2;
                    break;
                }
                default:
                    assert(false && "unsupported op");
            }
            init_number.push_back(resDouble);//add by yx
            Value* value = ConstantFP::get(builder.getDoubleTy(), resDouble);
            auto res_pair = insertSymbol(SymbolKind::kFP64Const, expr, value, 0);
            return res_pair.first;
        }
        else{
            double numeral = std::stod(numeral_str);
            init_number.push_back(numeral);
            Value* value = ConstantFP::get(builder.getDoubleTy(),numeral);
            auto res_pair = insertSymbol(SymbolKind::kFP64Const, expr, value, 0);
            return res_pair.first;
        }

    }

//    if (expr.decl().decl_kind() == Z3_OP_BNUM) {
//        auto result_iter = findSymbol(SymbolKind::kFP64Const, &expr);
//        if (result_iter != m_expr_sym_map.cend()) {
//            return &(*result_iter).second;
//        }
//        std::string numeral_str = Z3_ast_to_string(expr.ctx(),
//                                                   static_cast<z3::ast>(expr));
//        numeral_str.replace(0, 1, 1, '0');
//        double numeral = std::stod(numeral_str);
//        init_number.push_back(numeral);
//        Value* value = ConstantFP::get(builder.getDoubleTy(), numeral);
//        auto res_pair = insertSymbol(SymbolKind::kFP64Const, expr, value, 0);
//        return res_pair.first;
//    }

    return nullptr;
}

llvm::Function* FPIRGenerator::getDistanceFunction() const noexcept
{
    return m_func_fp64_dis;
}

llvm::Function* FPIRGenerator::genFunction
        (const z3::expr& expr, std::vector<double>& init_number)  noexcept
{
    using namespace llvm;
    if (m_gofunc != nullptr) {
        return m_gofunc;
    }
    m_gofunc = cast<Function>(
            m_mod->getOrInsertFunction(StringRef(CodeGenStr::kFunName),
                                       Type::getDoubleTy(*m_ctx),
//                                        Type::getInt8PtrTy(*m_ctx),
//                                       StructType::create({Type::getDoubleTy(*m_ctx),Type::getDoubleTy(*m_ctx)})->getPointerTo(),
                                       Type::getInt32Ty(*m_ctx),
                                       Type::getDoublePtrTy(*m_ctx),
                                       Type::getDoublePtrTy(*m_ctx),
                                       Type::getDoublePtrTy(*m_ctx)));


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
//    cur_arg++;
//    (*cur_arg).setName("cov");
//    cur_arg++;
//    (*cur_arg).setName("dis");
//    cur_arg++;
//    (*cur_arg).setName("func");

//  llvm::outs()<<"[add by yx] m_gofunc2:\n";
//  m_gofunc->print(llvm::outs());
//  llvm::outs()<<"\n";
//  [add by yx] m_gofunc2:
//
//  declare double @gofunc(i32, double* nocapture readonly, double* nocapture readnone, i8* nocapture readnone)

    BasicBlock* BB = BasicBlock::Create(*m_ctx, "EntryBlock", m_gofunc);
    IRBuilder<> builder(BB);

//    // add by yx
//    BasicBlock* trueBB = BasicBlock::Create(*m_ctx, "trueBlock", m_gofunc);
//    IRBuilder<> builder_true(trueBB);

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
    Type *FloatTY = Type::getFloatTy(*m_ctx);
    Type *BoolTY = Type::getInt1Ty(*m_ctx);
    Type *Int32TY = Type::getInt32Ty(*m_ctx);

    Type *DoublePtrTy = Type::getDoublePtrTy(*m_ctx);
//    const Type VecTy = Type::getVectorElementType(*m_ctx);
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

    m_func_fp32_dis = cast<Function>(
            m_mod->getOrInsertFunction(StringRef(CodeGenStr::kFun32Dis),
                                       FloatTY,FloatTY,FloatTY));

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
    m_func_distinct = cast<Function>(
            m_mod->getOrInsertFunction(StringRef(CodeGenStr::kFunDistinct),
                                       DoubleTY,DoubleTY,DoubleTY,Int32TY));
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
    m_func_distinct->setLinkage(Function::ExternalLinkage); // add by yx
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
//    m_const_min_double = ConstantFP::get(builder.getDoubleTy(), 1.0/__FP_LONG_MAX);
//    m_const_max_double = ConstantFP::get(builder.getDoubleTy(), MAXFLOAT);

//    //add by yx writing trueBlock
//    Argument* cov_arg = &(*(m_gofunc->arg_begin()+2));
    llvm::Value* cov = builder.CreateAlloca(Type::getDoubleTy(*m_ctx), nullptr, "cov");
    llvm::Value* totalCov = builder.CreateAlloca(Type::getDoubleTy(*m_ctx), nullptr, "totalCov");
    builder.CreateStore(m_const_zero, cov);
    builder.CreateStore(m_const_zero, totalCov);
//    builder.CreateAlloca()

//    llvm::outs()<<"[add by yx] m_gofunc2:\n";
//    m_gofunc->print(llvm::outs());
//    llvm::outs()<<"\n";

    //这条语句写了entryblock

    auto return_val_sym = genFuncRecursive(builder, expr, false, cov, totalCov, init_number);
//      llvm::outs()<<"[add by yx] m_gofunc7:\n";
//      m_gofunc->print(llvm::outs());
//      llvm::outs()<<"\n";
//      llvm::outs()<<*return_val_sym->getValue()<<"\n";
//    builder.CreateRet(return_val_sym->getValue());

    // add by yx
    Argument* grad_arg = &(*(m_gofunc->arg_begin()+2));
    Argument* data_arg = &(*(m_gofunc->arg_begin()+3));
    llvm::Value* loadCov = builder.CreateLoad(cov);
    llvm::Value* loadTotalCov = builder.CreateLoad(totalCov);

//    llvm::Value* covObj = builder.CreateFSub(loadTotalCov, loadCov); //totalCov - cov
//    llvm::Value* funcV = builder.CreateFAdd(covObj, return_val_sym->getValue()); totalCov - cov + distance

    builder.CreateStore(loadCov,grad_arg); //cov
    builder.CreateStore(loadTotalCov, data_arg); //totalcov
    builder.CreateRet(return_val_sym->getValue());

//    llvm::outs()<<"[add by yx] m_gofunc8:\n";
//    m_gofunc->print(llvm::outs());
//    llvm::outs()<<"\n";

    return m_gofunc;
}

//llvm::Function* FPIRGenerator::genFunction_int
//        (const z3::expr& expr, std::vector<int>& init_number)  noexcept
//{
//    using namespace llvm;
//    if (m_gofunc != nullptr) {
//        return m_gofunc;
//    }
//    m_gofunc = cast<Function>(
//            m_mod->getOrInsertFunction(StringRef(CodeGenStr::kFunName),
//                                       Type::getDoubleTy(*m_ctx),
////                                        Type::getInt8PtrTy(*m_ctx),
////                                       StructType::create({Type::getDoubleTy(*m_ctx),Type::getDoubleTy(*m_ctx)})->getPointerTo(),
//                                       Type::getInt32Ty(*m_ctx),
//                                       Type::getInt64PtrTy(*m_ctx),
//                                       Type::getDoublePtrTy(*m_ctx),
//                                       Type::getDoublePtrTy(*m_ctx)));
//
//
////    llvm::outs()<<"[add by yx] m_gofunc:\n";
////    m_gofunc->print(llvm::outs());
////    llvm::outs()<<"\n";
//    //declare double @gofunc(i32, double*, double*, i8*)
//
//    Function::arg_iterator cur_arg = m_gofunc->arg_begin();
//
////    llvm::outs()<<"[add by yx] cur_arg:\n";
////    cur_arg->print(llvm::outs());
////    llvm::outs()<<"\n";
//
////  [add by yx] cur_arg:
////  i32 %0
////  double* %0
////  double* %0
//
//    //给每个参数设置相应的属性
//    (*cur_arg).setName("n");
//    cur_arg++;
//    (*cur_arg).setName("x");
//    (*cur_arg).addAttr(Attribute::NoCapture);
//    (*cur_arg).addAttr(Attribute::ReadOnly);
//    cur_arg++;
//    (*cur_arg).setName("grad");
//    (*cur_arg).addAttr(Attribute::NoCapture);
//    (*cur_arg).addAttr(Attribute::ReadNone);
//    cur_arg++;
//    (*cur_arg).setName("data");
//    (*cur_arg).addAttr(Attribute::NoCapture);
//    (*cur_arg).addAttr(Attribute::ReadNone);
////    cur_arg++;
////    (*cur_arg).setName("cov");
////    cur_arg++;
////    (*cur_arg).setName("dis");
////    cur_arg++;
////    (*cur_arg).setName("func");
//
////  llvm::outs()<<"[add by yx] m_gofunc2:\n";
////  m_gofunc->print(llvm::outs());
////  llvm::outs()<<"\n";
////  [add by yx] m_gofunc2:
////
////  declare double @gofunc(i32, double* nocapture readonly, double* nocapture readnone, i8* nocapture readnone)
//
//    BasicBlock* BB = BasicBlock::Create(*m_ctx, "EntryBlock", m_gofunc);
//    IRBuilder<> builder(BB);
//
////    // add by yx
////    BasicBlock* trueBB = BasicBlock::Create(*m_ctx, "trueBlock", m_gofunc);
////    IRBuilder<> builder_true(trueBB);
//
//    // TBAA Metadata
//    MDBuilder md_builder(*m_ctx);
//    auto md_scalar = md_builder.createConstant(builder.getInt64(0));
//    auto md_node_1 = MDNode::get(*m_ctx,
//                                 MDString::get(*m_ctx, "Simple C/C++ TBAA"));
//    auto md_node_2 = MDNode::get(*m_ctx,
//                                 {MDString::get(*m_ctx, "omnipotent char"),
//                                  md_node_1, md_scalar});
//    auto md_node_3 = MDNode::get(*m_ctx,
//                                 {MDString::get(*m_ctx, "double"), md_node_2,
//                                  md_scalar});
//    m_tbaa_node = MDNode::get(*m_ctx, {md_node_3, md_node_3, md_scalar});
//
//    // Initialize external functions to be linked to JIT
//    Type *DoubleTY = Type::getDoubleTy(*m_ctx);
//    Type *BoolTY = Type::getInt1Ty(*m_ctx);
//    Type *Int32TY = Type::getInt32Ty(*m_ctx);
//    Type *Int64TY = Type::getInt64Ty(*m_ctx);
//
//    Type *DoublePtrTy = Type::getDoublePtrTy(*m_ctx);
////    const Type VecTy = Type::getVectorElementType(*m_ctx);
////  llvm::outs()<<"[add by yx] m_gofunc3:\n";
////  m_gofunc->print(llvm::outs());
////  llvm::outs()<<"\n";
//    //  [add by yx] m_gofunc3:
////
////  define double @gofunc(i32 %n, double* nocapture readonly %x, double* nocapture readnone %grad, i8* nocapture readnone %data) {
////    EntryBlock:
////  }
//
//    m_func_fp64_dis = cast<Function>(
//            m_mod->getOrInsertFunction(StringRef(CodeGenStr::kFunDis),
//                                       DoubleTY,DoubleTY,DoubleTY));
//
//    m_func_fp64_gt_dis = cast<Function>(
//            m_mod->getOrInsertFunction(StringRef(CodeGenStr::kFunGtDis),
//                                       DoubleTY,DoubleTY,DoubleTY));
//
//    m_func_fp64_lt_dis = cast<Function>(
//            m_mod->getOrInsertFunction(StringRef(CodeGenStr::kFunLtDis),
//                                       DoubleTY,DoubleTY,DoubleTY));
//
//    m_func_fp64_le_dis = cast<Function>(
//            m_mod->getOrInsertFunction(StringRef(CodeGenStr::kFunLeDis),
//                                       DoubleTY,DoubleTY,DoubleTY));
//
//    m_func_fp64_ge_dis = cast<Function>(
//            m_mod->getOrInsertFunction(StringRef(CodeGenStr::kFunGeDis),
//                                       DoubleTY,DoubleTY,DoubleTY));
//
//    m_func_fp64_overflow_dis = cast<Function>(
//            m_mod->getOrInsertFunction(StringRef(CodeGenStr::kFunOverflowDis),
//                                       DoubleTY,DoubleTY,DoubleTY,DoubleTY,DoubleTY));
////    llvm::outs()<<"[add by yx]\n";
////    m_func_fp64_dis->print(llvm::outs());
////    llvm::outs()<<"\n";
//
//    m_func_fp64_eq_dis = cast<Function>(
//            m_mod->getOrInsertFunction(StringRef(CodeGenStr::kFunEqDis),
//                                       DoubleTY,DoubleTY,DoubleTY));
//
////    llvm::outs()<<"[add by yx]\n";
////    m_func_fp64_eq_dis->print(llvm::outs());
////    llvm::outs()<<"\n";
//
//    m_func_fp64_neq_dis = cast<Function>(
//            m_mod->getOrInsertFunction(StringRef(CodeGenStr::kFunNEqDis),
//                                       DoubleTY,DoubleTY,DoubleTY));
//    m_func_isnan = cast<Function>(
//            m_mod->getOrInsertFunction(StringRef(CodeGenStr::kFunIsNan),
//                                       DoubleTY,DoubleTY,DoubleTY));
//    // add by zgf
//    m_func_isinf = cast<Function>(
//            m_mod->getOrInsertFunction(StringRef(CodeGenStr::kFunIsInf),
//                                       DoubleTY,DoubleTY,DoubleTY));
//    m_func_ite = cast<Function>(
//            m_mod->getOrInsertFunction(StringRef(CodeGenStr::kFunIte),
//                                       DoubleTY,DoubleTY,DoubleTY,DoubleTY));
//    m_func_distinct = cast<Function>(
//            m_mod->getOrInsertFunction(StringRef(CodeGenStr::kFunDistinct),
//                                       DoubleTY,DoubleTY,DoubleTY,Int32TY));
//    m_func_band = cast<Function>(
//            m_mod->getOrInsertFunction(StringRef(CodeGenStr::kFunBand),
//                                       DoubleTY,DoubleTY,DoubleTY));
//    m_func_bor = cast<Function>(
//            m_mod->getOrInsertFunction(StringRef(CodeGenStr::kFunBor),
//                                       DoubleTY,DoubleTY,DoubleTY));
//    m_func_bxor = cast<Function>(
//            m_mod->getOrInsertFunction(StringRef(CodeGenStr::kFunBxor),
//                                       DoubleTY,DoubleTY,DoubleTY));
//
//    // add by zgf : one arg
//    m_func_tan = cast<Function>(
//            m_mod->getOrInsertFunction(StringRef(CodeGenStr::kFunTan),
//                                       DoubleTY,DoubleTY));
//    m_func_asin = cast<Function>(
//            m_mod->getOrInsertFunction(StringRef(CodeGenStr::kFunASin),
//                                       DoubleTY,DoubleTY));
//    m_func_acos = cast<Function>(
//            m_mod->getOrInsertFunction(StringRef(CodeGenStr::kFunACos),
//                                       DoubleTY,DoubleTY));
//    m_func_atan = cast<Function>(
//            m_mod->getOrInsertFunction(StringRef(CodeGenStr::kFunATan),
//                                       DoubleTY,DoubleTY));
//    m_func_sinh = cast<Function>(
//            m_mod->getOrInsertFunction(StringRef(CodeGenStr::kFunSinh),
//                                       DoubleTY,DoubleTY));
//    m_func_cosh = cast<Function>(
//            m_mod->getOrInsertFunction(StringRef(CodeGenStr::kFunCosh),
//                                       DoubleTY,DoubleTY));
//    m_func_tanh = cast<Function>(
//            m_mod->getOrInsertFunction(StringRef(CodeGenStr::kFunTanh),
//                                       DoubleTY,DoubleTY));
//    // add by zgf : two arg
//    m_func_pow = cast<Function>(
//            m_mod->getOrInsertFunction(StringRef(CodeGenStr::kFunPow),
//                                       DoubleTY,DoubleTY,DoubleTY));
//    m_func_atan2 = cast<Function>(
//            m_mod->getOrInsertFunction(StringRef(CodeGenStr::kFunATan2),
//                                       DoubleTY,DoubleTY,DoubleTY));
//    m_func_fmin = cast<Function>(
//            m_mod->getOrInsertFunction(StringRef(CodeGenStr::kFunFMin),
//                                       DoubleTY,DoubleTY,DoubleTY));
//    m_func_fmax = cast<Function>(
//            m_mod->getOrInsertFunction(StringRef(CodeGenStr::kFunFMax),
//                                       DoubleTY,DoubleTY,DoubleTY));
//
//    m_func_fpcheck_dis = cast<Function>(
//            m_mod->getOrInsertFunction(StringRef(CodeGenStr::kFunFPDis),
//                                       DoubleTY,DoubleTY,DoubleTY,Int32TY,Int32TY));
//    m_func_fpcheck_fadd_overflow = cast<Function>(
//            m_mod->getOrInsertFunction(StringRef(CodeGenStr::kFunFAddOverflow),
//                                       BoolTY,DoubleTY,DoubleTY));
//    m_func_fpcheck_fsub_overflow = cast<Function>(
//            m_mod->getOrInsertFunction(StringRef(CodeGenStr::kFunFSubOverflow),
//                                       BoolTY,DoubleTY,DoubleTY));
//    m_func_fpcheck_fmul_overflow = cast<Function>(
//            m_mod->getOrInsertFunction(StringRef(CodeGenStr::kFunFMulOverflow),
//                                       BoolTY,DoubleTY,DoubleTY));
//    m_func_fpcheck_fdiv_overflow = cast<Function>(
//            m_mod->getOrInsertFunction(StringRef(CodeGenStr::kFunFDivOverflow),
//                                       BoolTY,DoubleTY,DoubleTY));
//    m_func_fpcheck_fadd_underflow = cast<Function>(
//            m_mod->getOrInsertFunction(StringRef(CodeGenStr::kFunFAddUnderflow),
//                                       BoolTY,DoubleTY,DoubleTY));
//    m_func_fpcheck_fsub_underflow = cast<Function>(
//            m_mod->getOrInsertFunction(StringRef(CodeGenStr::kFunFSubUnderflow),
//                                       BoolTY,DoubleTY,DoubleTY));
//    m_func_fpcheck_fmul_underflow = cast<Function>(
//            m_mod->getOrInsertFunction(StringRef(CodeGenStr::kFunFMulUnderflow),
//                                       BoolTY,DoubleTY,DoubleTY));
//    m_func_fpcheck_fdiv_underflow = cast<Function>(
//            m_mod->getOrInsertFunction(StringRef(CodeGenStr::kFunFDivUnderflow),
//                                       BoolTY,DoubleTY,DoubleTY));
//    m_func_fpcheck_fdiv_invalid = cast<Function>(
//            m_mod->getOrInsertFunction(StringRef(CodeGenStr::kFunFDivInvalid),
//                                       BoolTY,DoubleTY,DoubleTY));
//    m_func_fpcheck_fdiv_zero = cast<Function>(
//            m_mod->getOrInsertFunction(StringRef(CodeGenStr::kFunFDivZero),
//                                       BoolTY,DoubleTY,DoubleTY));
////    add by yx
//    m_func_fpcheck_finvalid_sqrt = cast<Function>(
//            m_mod->getOrInsertFunction(StringRef(CodeGenStr::kFunFInvalidSqrt),
//                                       BoolTY,DoubleTY,DoubleTY));
//    m_func_fpcheck_finvalid_log = cast<Function>(
//            m_mod->getOrInsertFunction(StringRef(CodeGenStr::kFunFInvalidLog),
//                                       BoolTY,DoubleTY,DoubleTY));
//    m_func_fpcheck_finvalid_pow = cast<Function>(
//            m_mod->getOrInsertFunction(StringRef(CodeGenStr::kFunFInvalidPow),
//                                       BoolTY,DoubleTY,DoubleTY));
//    m_func_fpcheck_fadd_accuracy = cast<Function>(
//            m_mod->getOrInsertFunction(StringRef(CodeGenStr::kFunFAddAccuracy),
//                                       BoolTY,DoubleTY,DoubleTY));
//    m_func_fpcheck_fsub_accuracy = cast<Function>(
//            m_mod->getOrInsertFunction(StringRef(CodeGenStr::kFunFSubAccuracy),
//                                       BoolTY,DoubleTY,DoubleTY));
//    m_func_fpcheck_fmul_accuracy = cast<Function>(
//            m_mod->getOrInsertFunction(StringRef(CodeGenStr::kFunFMulAccuracy),
//                                       BoolTY,DoubleTY,DoubleTY));
//    m_func_fpcheck_fdiv_accuracy = cast<Function>(
//            m_mod->getOrInsertFunction(StringRef(CodeGenStr::kFunFDivAccuracy),
//                                       BoolTY,DoubleTY,DoubleTY));
////  llvm::outs()<<"[add by yx] m_gofunc4:\n";
////  m_gofunc->print(llvm::outs());
////  llvm::outs()<<"\n";
////  [add by yx] m_gofunc4:
////
////  define double @gofunc(i32 %n, double* nocapture readonly %x, double* nocapture readnone %grad, i8* nocapture readnone %data) {
////    EntryBlock:
////  }
//
//    m_func_fp64_dis->setLinkage(Function::ExternalLinkage);
////    [add by yx]
//    m_func_fp64_gt_dis->setLinkage(Function::ExternalLinkage);
//    m_func_fp64_lt_dis->setLinkage(Function::ExternalLinkage);
//    m_func_fp64_ge_dis->setLinkage(Function::ExternalLinkage);
//    m_func_fp64_le_dis->setLinkage(Function::ExternalLinkage);
//    m_func_fp64_overflow_dis->setLinkage(Function::ExternalLinkage);
//    m_func_fp64_eq_dis->setLinkage(Function::ExternalLinkage);
//    m_func_fp64_neq_dis->setLinkage(Function::ExternalLinkage);
//    m_func_isnan->setLinkage(Function::ExternalLinkage);
//    // add by zgf
//    m_func_isinf->setLinkage(Function::ExternalLinkage);
//    m_func_ite->setLinkage(Function::ExternalLinkage);
//    m_func_distinct->setLinkage(Function::ExternalLinkage); // add by yx
//    m_func_band->setLinkage(Function::ExternalLinkage);
//    m_func_bor->setLinkage(Function::ExternalLinkage);
//    m_func_bxor->setLinkage(Function::ExternalLinkage);
//    m_func_tan->setLinkage(Function::ExternalLinkage);
//    m_func_asin->setLinkage(Function::ExternalLinkage);
//    m_func_acos->setLinkage(Function::ExternalLinkage);
//    m_func_atan->setLinkage(Function::ExternalLinkage);
//    m_func_sinh->setLinkage(Function::ExternalLinkage);
//    m_func_cosh->setLinkage(Function::ExternalLinkage);
//    m_func_tanh->setLinkage(Function::ExternalLinkage);
//    m_func_pow->setLinkage(Function::ExternalLinkage);
//    m_func_atan2->setLinkage(Function::ExternalLinkage);
//    m_func_fmin->setLinkage(Function::ExternalLinkage);
//    m_func_fmax->setLinkage(Function::ExternalLinkage);
//
//    m_func_fpcheck_dis->setLinkage(Function::ExternalLinkage);
//    m_func_fpcheck_fadd_overflow->setLinkage(Function::ExternalLinkage);
//    m_func_fpcheck_fsub_overflow->setLinkage(Function::ExternalLinkage);
//    m_func_fpcheck_fmul_overflow->setLinkage(Function::ExternalLinkage);
//    m_func_fpcheck_fdiv_overflow->setLinkage(Function::ExternalLinkage);
//    m_func_fpcheck_fadd_underflow->setLinkage(Function::ExternalLinkage);
//    m_func_fpcheck_fsub_underflow->setLinkage(Function::ExternalLinkage);
//    m_func_fpcheck_fmul_underflow->setLinkage(Function::ExternalLinkage);
//    m_func_fpcheck_fdiv_underflow->setLinkage(Function::ExternalLinkage);
//    m_func_fpcheck_fdiv_invalid->setLinkage(Function::ExternalLinkage);
//    m_func_fpcheck_fdiv_zero->setLinkage(Function::ExternalLinkage);
////add by yx
//    m_func_fpcheck_finvalid_sqrt->setLinkage(Function::ExternalLinkage);
//    m_func_fpcheck_finvalid_log->setLinkage(Function::ExternalLinkage);
//    m_func_fpcheck_finvalid_pow->setLinkage(Function::ExternalLinkage);
//    m_func_fpcheck_fadd_accuracy->setLinkage(Function::ExternalLinkage);
//    m_func_fpcheck_fsub_accuracy->setLinkage(Function::ExternalLinkage);
//    m_func_fpcheck_fmul_accuracy->setLinkage(Function::ExternalLinkage);
//    m_func_fpcheck_fdiv_accuracy->setLinkage(Function::ExternalLinkage);
//
////  llvm::outs()<<"[add by yx] m_gofunc5:\n";
////  m_gofunc->print(llvm::outs());
////  llvm::outs()<<"\n";
////
////  [add by yx] m_gofunc5:
////
////  define double @gofunc(i32 %n, double* nocapture readonly %x, double* nocapture readnone %grad, i8* nocapture readnone %data) {
////    EntryBlock:
////  }
//    m_const_zero = ConstantFP::get(builder.getDoubleTy(), 0.0);
//    m_const_one = ConstantFP::get(builder.getDoubleTy(), 1.0);
////    m_const_min_double = ConstantFP::get(builder.getDoubleTy(), 1.0/__FP_LONG_MAX);
////    m_const_max_double = ConstantFP::get(builder.getDoubleTy(), MAXFLOAT);
//
////    //add by yx writing trueBlock
////    Argument* cov_arg = &(*(m_gofunc->arg_begin()+2));
//    llvm::Value* cov = builder.CreateAlloca(Type::getDoubleTy(*m_ctx), nullptr, "cov");
//    llvm::Value* totalCov = builder.CreateAlloca(Type::getDoubleTy(*m_ctx), nullptr, "totalCov");
//    builder.CreateStore(m_const_zero, cov);
//    builder.CreateStore(m_const_zero, totalCov);
////    builder.CreateAlloca()
//
////    llvm::outs()<<"[add by yx] m_gofunc2:\n";
////    m_gofunc->print(llvm::outs());
////    llvm::outs()<<"\n";
//
//    //这条语句写了entryblock
//
//    auto return_val_sym = genFuncRecursive(builder, expr, false, cov, totalCov, init_number);
////      llvm::outs()<<"[add by yx] m_gofunc7:\n";
////      m_gofunc->print(llvm::outs());
////      llvm::outs()<<"\n";
////      llvm::outs()<<*return_val_sym->getValue()<<"\n";
////    builder.CreateRet(return_val_sym->getValue());
//
//    // add by yx
//    Argument* grad_arg = &(*(m_gofunc->arg_begin()+2));
//    Argument* data_arg = &(*(m_gofunc->arg_begin()+3));
//    llvm::Value* loadCov = builder.CreateLoad(cov);
//    llvm::Value* loadTotalCov = builder.CreateLoad(totalCov);
//
////    llvm::Value* covObj = builder.CreateFSub(loadTotalCov, loadCov); //totalCov - cov
////    llvm::Value* funcV = builder.CreateFAdd(covObj, return_val_sym->getValue()); totalCov - cov + distance
//
//    builder.CreateStore(loadCov,grad_arg); //cov
//    builder.CreateStore(loadTotalCov, data_arg); //totalcov
//    builder.CreateRet(return_val_sym->getValue());
//
////    llvm::outs()<<"[add by yx] m_gofunc8:\n";
////    m_gofunc->print(llvm::outs());
////    llvm::outs()<<"\n";
//
//    return m_gofunc;
//}

const IRSymbol* FPIRGenerator::genFuncRecursive
        (llvm::IRBuilder<>& builder, const z3::expr expr, bool is_negated, llvm::Value* cov, llvm::Value* totalCov,
         std::vector<double>& init_number) noexcept
{
//    std::cerr << "[yx dbg] expr : \n"<<expr.to_string()<<"\n";
    if (!expr.is_app()) {
        // is_app <==> Z3_NUMERAL_AST || Z3_APP_AST
        std::cerr << "unsupported\n";
        assert(false && "unsupport expr !");
    }
    if (fpa_util::isRoundingModeApp(expr) &&
        expr.decl().decl_kind() != Z3_OP_FPA_RM_NEAREST_TIES_TO_EVEN &&
        expr.decl().decl_kind() != Z3_OP_FPA_RM_TOWARD_ZERO &&
        expr.decl().decl_kind() != Z3_OP_FPA_RM_TOWARD_POSITIVE &&
        expr.decl().decl_kind() != Z3_OP_FPA_RM_TOWARD_NEGATIVE) {
        m_found_unsupported_smt_expr = true;
        assert(false && "unsupport expr !");
    }
    if(expr.get_sort().sort_kind()==Z3_ROUNDING_MODE_SORT){
        return nullptr;
    }
    if (expr.is_numeral()) {
//        llvm::outs()<<"[add by yx]:\n"<<expr.to_string()<<"\n";
        return genNumeralIR(builder, expr, init_number);
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
//        std::cerr << "[yx dbg] expr : \n"<<expr.to_string()<<"\n";
//        decl_list.push_back(expr.decl());
//        symbol_list.push_back(expr.decl().to_string());
//        sort_list.push_back(expr.get_sort());
//        kind_list.push_back(expr.get_sort().sort_kind());

//        Z3_fpa_get_sbits(expr.ctx(), expr.get_sort());
//        Z3_sort sort = Z3_get_sort(expr.ctx(), expr);
//        llvm::outs()<<sort<<"\n";
//        Z3_sort_kind sort_kind = Z3_get_sort_kind(expr.ctx(), sort_list[0]);
//        llvm::outs()<<sort_kind<<"\n";

        using namespace llvm;
        Argument* x_arg = &(*(m_gofunc->arg_begin()+1));

        auto idx_ptr = builder.CreateInBoundsGEP
                (llvm::cast<Value>(x_arg),
                 builder.getInt64(getVarCount()));
        auto loaded_val = builder.CreateAlignedLoad(idx_ptr, 8);
        loaded_val->setMetadata(llvm::LLVMContext::MD_tbaa, m_tbaa_node);
        auto result_pair =
                insertSymbol(kind, expr, loaded_val, getVarCount());
        m_var_sym_vec.emplace_back(result_pair.first);

        return result_pair.first;
    }
    //add by yx
    if (fpa_util::isREALVar(expr)) {
        SymbolKind kind=SymbolKind::kFP64Const;
        auto result_iter = findSymbol(kind, &expr);
        if (result_iter != m_expr_sym_map.cend()) {
            return &(*result_iter).second;
        }

        using namespace llvm;
        Argument* x_arg = &(*(m_gofunc->arg_begin()+1));

        auto idx_ptr = builder.CreateInBoundsGEP
                (llvm::cast<Value>(x_arg),
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
//    if (fpa_util::isBoolExpr(expr) || expr.decl().decl_kind() == Z3_OP_NOT) {
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
//        llvm::outs()<<"expr>>>\n"<<expr.arg(i).to_string()<<"\n";
//        if(expr.arg(i).decl().decl_kind() >= Z3_OP_FPA_RM_NEAREST_TIES_TO_EVEN &&
//            expr.arg(i).decl().decl_kind() <= Z3_OP_FPA_RM_TOWARD_ZERO){p
//            continue;
//        }
        auto arg_sym = genFuncRecursive(builder, expr.arg(i), is_negated, cov, totalCov, init_number);
        arg_syms.push_back(arg_sym);
//        llvm::outs()<<arg_sym->expr()->to_string()<<"\n";
    }
    auto res_pair = insertSymbol(kind, expr, nullptr);
    res_pair.first->setValue(genExprIR(builder, res_pair.first, arg_syms, cov, totalCov, init_number));
//    llvm::outs()<<"IR>>>\n"<<*res_pair.first->getValue()<<"\n";
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
    //  add by yx
    if((expr.decl().decl_kind() >= Z3_OP_FPA_EQ && expr.decl().decl_kind() <= Z3_OP_FPA_IS_POSITIVE) ||
            (expr.decl().decl_kind() >= Z3_OP_LE && expr.decl().decl_kind() <= Z3_OP_GT) ||
            (expr.decl().decl_kind() == Z3_OP_EQ)){
        builder.CreateStore(builder.CreateFAdd(builder.CreateLoad(totalCov), m_const_one), totalCov);

        llvm::BasicBlock* bb_true = llvm::BasicBlock::Create(*m_ctx, "", m_gofunc);
        llvm::BasicBlock* bb_false = llvm::BasicBlock::Create(*m_ctx, "", m_gofunc);
//        llvm::BasicBlock* bb_cur = builder.GetInsertBlock();

//        llvm::Argument* cov_arg = &(*(m_gofunc->arg_begin()+2));
        auto comp_res = builder.CreateFCmpOLE(res_pair.first->getValue(), m_const_zero);
        builder.CreateCondBr(comp_res, bb_true, bb_false);

        builder.SetInsertPoint(bb_true);
        llvm::Value* load_cov_value = builder.CreateLoad(cov);
        llvm::Value* new_cov_value = builder.CreateFAdd(load_cov_value, m_const_one);
        builder.CreateStore(new_cov_value, cov);
        builder.CreateBr(bb_false);

        builder.SetInsertPoint(bb_false);
    }
//add by yx
//    if(expr.decl().decl_kind()==Z3_OP_AND){
//        for(unsigned i=0; i<arg_syms.size(); i++){
//            llvm::Value* cond = builder.CreateFCmpOLE(arg_syms[i]->getValue(), m_const_zero);
//            builder.CreateCondBr(cond, trueBB, BB);
//        }
//    }
//    llvm::outs()<<"[add by yx] m_gofunc:\n";
//    m_gofunc->print(llvm::outs());
//    llvm::outs()<<"\n";

    return res_pair.first;
}

//[yx flag]
llvm::Value* FPIRGenerator::genExprIR
        (llvm::IRBuilder<>& builder, const IRSymbol* expr_sym, std::vector<const IRSymbol*>& arg_syms,
         llvm::Value* cov, llvm::Value* totalCov, std::vector<double> &init_number) noexcept
{
//    llvm::outs()<<"genExprIR>>>\n"<<expr_sym->isNegated()<<" "<<expr_sym->expr()->decl().name().str()<<"\n";
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
        case Z3_OP_EQ:{
            // llvm::errs()<<"[add by yx] Z3_OP_EQ\n";
        }
        case Z3_OP_FPA_EQ:
            return genEqualityIR(builder, expr_sym, arg_syms);
        case Z3_OP_NOT:
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

//        // Real operations
//        case Z3_OP_UMINUS:
//            return ConstantFP::get(builder.getDoubleTy(), -arg_syms[0]->getValue());

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
        case Z3_OP_BADD:{
            // llvm::errs()<<"[add by yx] Z3_OP_BADD\n";
        }
        case Z3_OP_ADD:{ // add by yx
//            return builder.CreateFAdd(arg_syms[0]->getValue(),
//                                      arg_syms[1]->getValue());
            auto result = builder.CreateFAdd(arg_syms[0]->getValue(),
                                             arg_syms[1]->getValue());
            for (unsigned i = 2; i < arg_syms.size(); ++i) {
                result = builder.CreateFAdd(result, arg_syms[i]->getValue());
            }
            return result;
        }
        case Z3_OP_FPA_ADD:{
            return builder.CreateFAdd(arg_syms[1]->getValue(),
                                      arg_syms[2]->getValue());

//            auto result = builder.CreateFAdd(arg_syms[1]->getValue(),
//                                             arg_syms[2]->getValue());
//            for (unsigned i = 3; i < arg_syms.size(); ++i) {
//                result = builder.CreateFAdd(result, arg_syms[i]->getValue());
//            }
        }
        case Z3_OP_BSUB:{
            // llvm::errs()<<"[add by yx] Z3_OP_BSUB\n";
        }
        case Z3_OP_SUB: { // add by yx
//            return builder.CreateFSub(arg_syms[0]->getValue(),
//                                      arg_syms[1]->getValue());
            auto result = builder.CreateFSub(arg_syms[0]->getValue(),
                                             arg_syms[1]->getValue());
            for (unsigned i = 2; i < arg_syms.size(); ++i) {
                result = builder.CreateFSub(result, arg_syms[i]->getValue());
            }
            return result;
        }
        case Z3_OP_FPA_SUB:
            return builder.CreateFSub(arg_syms[1]->getValue(),
                                      arg_syms[2]->getValue());

        case Z3_OP_BNEG:{
            // llvm::errs()<<"unsupport BNEG\n";
        }
        case Z3_OP_FPA_NEG:{
            // llvm::errs()<<"unsupport FPA_NEG\n";
        }
        case Z3_OP_UMINUS: // add by yx
            return builder.CreateFSub(
                    ConstantFP::get(builder.getDoubleTy(), -0.0),
                    arg_syms[0]->getValue());
        case Z3_OP_BMUL:{
            // llvm::errs()<<"[add by yx] Z3_OP_BMUL\n";
        }
        case Z3_OP_MUL: {// add by yx
            auto result = builder.CreateFMul(arg_syms[0]->getValue(),
                                             arg_syms[1]->getValue());
            for (unsigned i = 2; i < arg_syms.size(); ++i) {
                result = builder.CreateFMul(result, arg_syms[i]->getValue());
            }
            return result;
        }
        case Z3_OP_FPA_MUL:{
            return builder.CreateFMul(arg_syms[1]->getValue(),
                                      arg_syms[2]->getValue());
        }
        case Z3_OP_BSDIV:{
            // llvm::errs()<<"[add by yx] Z3_OP_BSDIV\n";
        }
        case Z3_OP_BUDIV: {
//            return builder.CreateFDiv(arg_syms[0]->getValue(),
//                                      arg_syms[1]->getValue());
            auto result = builder.CreateFDiv(arg_syms[0]->getValue(),
                                             arg_syms[1]->getValue());
            for (unsigned i = 2; i < arg_syms.size(); ++i) {
                result = builder.CreateFDiv(result, arg_syms[i]->getValue());
            }
            return result;
        }
        case Z3_OP_FPA_DIV:
            return builder.CreateFDiv(arg_syms[1]->getValue(),
                                      arg_syms[2]->getValue());
        case Z3_OP_BSREM:{
            // llvm::errs()<<"[add by yx] Z3_OP_BSREM\n";
        }
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
        case Z3_OP_SLT:{
            // llvm::errs()<<"[into case Z3_OP_SLT]\n";
        }
        case Z3_OP_ULT:{
            // llvm::errs()<<"[into case Z3_OP_ULT]\n";
        }
        case Z3_OP_LT:{
            // llvm::errs()<<"[into case Z3_OP_LT]\n";
        }
        case Z3_OP_FPA_LT:{
            if (expr_sym->isNegated()) {//not a<b
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
        }
        case Z3_OP_SGT:{
            // llvm::errs()<<"[into case Z3_OP_SGT]\n";
        }
        case Z3_OP_UGT:{
            // llvm::errs()<<"[into case Z3_OP_UGT]\n";
        }
        case Z3_OP_GT: {
            // llvm::errs()<<"[into case Z3_OP_GT]\n";
        }
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
        case Z3_OP_SLEQ:{
            // llvm::errs()<<"[into case Z3_OP_SLEQ]\n";
        }
        case Z3_OP_ULEQ:{
            // llvm::errs()<<"[into case Z3_OP_ULEQ]\n";
        }
        case Z3_OP_LE:{
            // llvm::errs()<<"[into case Z3_OP_LE]\n";
        }
        case Z3_OP_FPA_LE:
            if (expr_sym->isNegated()) {
                auto comp_res = builder.CreateFCmpOGT(arg_syms[0]->getValue(),
                                                      arg_syms[1]->getValue());
                return genBinArgCmpIR2(builder, arg_syms, comp_res);
//                return genBinArgCmpIR3(builder, arg_syms, comp_res, Z3_OP_FPA_LE);
            } else {
                auto comp_res = builder.CreateFCmpOLE(arg_syms[0]->getValue(),
                                                      arg_syms[1]->getValue());
//                llvm::outs()<<comp_res->dump()<<"\n";
                return genBinArgCmpIR(builder, arg_syms, comp_res);
//                return genBinArgCmpIR3(builder, arg_syms, comp_res, Z3_OP_FPA_LE);
            }
        case Z3_OP_SGEQ:{
            // llvm::errs()<<"[into case Z3_OP_SGEQ]\n";
        }
        case Z3_OP_UGEQ:{
            // llvm::errs()<<"[into case Z3_OP_UGEQ]\n";
        }
        case Z3_OP_GE:{
            // llvm::errs()<<"[into case Z3_OP_GE]\n";
        }
        case Z3_OP_FPA_GE:
            if (expr_sym->isNegated()) {
                auto comp_res = builder.CreateFCmpOLT(arg_syms[0]->getValue(),
                                                      arg_syms[1]->getValue());
                return genBinArgCmpIR2(builder, arg_syms, comp_res);
//                return genBinArgCmpIR3(builder, arg_syms, comp_res, Z3_OP_FPA_GE);
            } else {
//                llvm::outs()<<"Z3_OP_FPA_GE\n";
//                llvm::outs()<<*arg_syms[0]->getValue()<<"\n";
//                llvm::outs()<<*arg_syms[1]->getValue()<<"\n";
                auto comp_res = builder.CreateFCmpOGE(arg_syms[0]->getValue(),
                                                      arg_syms[1]->getValue());
                return genBinArgCmpIR(builder, arg_syms, comp_res);
//                return genBinArgCmpIR3(builder, arg_syms, comp_res, Z3_OP_FPA_GE);
            }
        case Z3_OP_SIGN_EXT:{
            // llvm::errs()<<"unsupport SIGN_EXT\n";
//            std::string all_str = expr_sym->expr()->to_string();
//            std::vector<std::string> lines;
//            std::istringstream iss(all_str);
//            std::string line;
//            while (std::getline(iss, line)) {
//                lines.push_back(line);
//            }
//
//            std::vector<std::string> tokens;
//            std::istringstream space_s(lines[0]);
//            std::string token;
//            while (space_s >> token) {  // 默认按空格分割
//                tokens.push_back(token);
//            }
////            std::string resStr = tokens[tokens.size()-1];
//            unsigned extend_to_bits = std::stoi(tokens[tokens.size()-1]);
//
//            // 获取源值
//            llvm::Value* src = arg_syms[0]->getValue();
//
//            // 处理整数类型
//            if (src->getType()->isIntegerTy()) {
//                unsigned src_bits = src->getType()->getIntegerBitWidth();
//                if (src_bits >= extend_to_bits) {
//                    llvm::errs() << "Can't sign-extend: source bitwidth " << src_bits
//                                 << " >= target " << extend_to_bits << "\n";
//                    return nullptr;
//                }
//                llvm::Type* dst_ty = llvm::IntegerType::get(builder.getContext(), extend_to_bits);
//                return builder.CreateSExt(src, dst_ty);
//            }
//                // 处理浮点类型（提取符号位）
//            else if (src->getType()->isFloatingPointTy()) {
//                llvm::Type* int_ty = src->getType()->isFloatTy() ?
//                                     builder.getInt32Ty() : builder.getInt64Ty();
//                llvm::Value* bits = builder.CreateBitCast(src, int_ty);
//                llvm::Value* sign_bit = builder.CreateLShr(bits, int_ty->getIntegerBitWidth() - 1);
//                sign_bit = builder.CreateTrunc(sign_bit, builder.getInt1Ty());
//                return builder.CreateSExt(sign_bit, builder.getInt32Ty());
//            }
//                // 错误处理
//            else {
//                llvm::errs() << "Unsupported type for sign_extend: ";
//                src->getType()->print(llvm::errs());
//                llvm::errs() << "\n";
//                return nullptr;
//            }
        }
        case Z3_OP_ZERO_EXT:{
            // llvm::errs()<<"unsupport ZERO_EXT\n";
        }
        case Z3_OP_FPA_TO_FP_UNSIGNED:
//        case Z3_OP_FPA_TO_FP:{
//            if (arg_syms[arg_syms.size() - 1]->kind() != SymbolKind::kFP32Const &&
//                arg_syms[arg_syms.size() - 1]->kind() != SymbolKind::kFP64Const) {
//                return arg_syms.back()->getValue();
//            }
//            // 处理从位向量到浮点的转换
//            if (arg_syms.back()->expr()->get_sort().sort_kind() == Z3_BV_SORT) {
//                llvm::Value* bv_val = arg_syms.back()->getValue();
//                unsigned manti_width = bv_val->getType()->getFPMantissaWidth();
//
//                if (manti_width == 24) {
//                    // 32位 -> 单精度浮点
//                    llvm::Type* floatTy = llvm::Type::getFloatTy(*m_ctx);
//                    return builder.CreateBitCast(bv_val, floatTy);
//                }
//                else if (manti_width == 53) {
//                    // 64位 -> 双精度浮点
//                    llvm::Type* doubleTy = llvm::Type::getDoubleTy(*m_ctx);
//                    return builder.CreateBitCast(bv_val, doubleTy);
//                }
//            }
//            // 其他情况保持原样
//            return arg_syms.back()->getValue();
//        }
        case Z3_OP_FPA_TO_FP: {
//            llvm::outs()<<"expr>>>"<<arg_syms[arg_syms.size()-1]->expr()->to_string()<<"\n";
            if (arg_syms[arg_syms.size() - 1]->kind() != SymbolKind::kFP32Const &&
                arg_syms[arg_syms.size() - 1]->kind() != SymbolKind::kFP64Const) {
//                llvm::outs()<<"getValue>>>"<<*arg_syms[arg_syms.size()-1]->getValue()<<"\n";
                return arg_syms[arg_syms.size() - 1]->getValue();
            }
//            if (arg_syms[arg_syms.size() - 1]->expr()->get_sort().sort_kind() == Z3_REAL_SORT) {
//                return arg_syms[arg_syms.size() - 1]->getValue();
//            }
            if (arg_syms[arg_syms.size() - 1]->expr()->get_sort().sort_kind() == Z3_BV_SORT) {
    //          llvm::outs()<<arg_syms[arg_syms.size()-1]->getValue()<<"\n";
                std::string numeral_str = arg_syms[arg_syms.size() - 1]->expr()->to_string();
                std::string hex_string = numeral_str.substr(2, numeral_str.size() - 2);//delete "#x"
    //          char* hex_string = "3fe0000000000000";
                uint64_t int_value;
                sscanf(hex_string.c_str(), "%lx", &int_value);
    //          double *numeral = (double*)&int_value;
                double numeral = 0.0;
                if (arg_syms[arg_syms.size() - 1]->kind() == SymbolKind::kFP32Const) {
                    float *numeral32 = (float *) &int_value;
                    numeral = *numeral32;
                } else if (arg_syms[arg_syms.size() - 1]->kind() == SymbolKind::kFP64Const) {
                    double *numeral64 = (double *) &int_value;
                    numeral = *numeral64;
                } else {
                    assert(true && "unsupport SymbolKind!");
                }

    //          llvm::outs()<<"numeral>>>"<<numeral<<"\n";
                init_number.push_back(numeral);
                Value *value = ConstantFP::get(builder.getDoubleTy(), numeral);
                return value;
            }
            return arg_syms[arg_syms.size()-1]->getValue();
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

        case Z3_OP_CONCAT:{
            // 获取所有参数的位向量值
            std::vector<llvm::Value*> bv_values;
            for (const auto& arg : arg_syms) {
                bv_values.push_back(arg->getValue());
            }

            // 确保至少有两个参数
            if (bv_values.size() < 2) {
                llvm::errs() << "Error: concat requires at least 2 arguments\n";
                return nullptr;
            }

            // 从最后一个参数开始，逐步向前连接
            llvm::Value* result = bv_values.back();
            for (int i = bv_values.size() - 2; i >= 0; --i) {
                // 获取当前位向量的位宽
                unsigned width = result->getType()->getIntegerBitWidth() +
                                 bv_values[i]->getType()->getIntegerBitWidth();

                // 扩展每个操作数到最终宽度
                llvm::Value* op1 = builder.CreateZExt(bv_values[i],
                                                      llvm::IntegerType::get(*m_ctx, width));
                llvm::Value* op2 = builder.CreateZExt(result,
                                                      llvm::IntegerType::get(*m_ctx, width));

                // 左移第一个操作数
                op1 = builder.CreateShl(op1,
                                        llvm::ConstantInt::get(op1->getType(),
                                                               result->getType()->getIntegerBitWidth()));

                // 执行或操作连接位向量
                result = builder.CreateOr(op1, op2);
            }

            return result;
        }

        case Z3_OP_EXTRACT:{
            // 确保有3个参数：(extract i j bv)
            if (arg_syms.size() != 3) {
                llvm::errs() << "Error: extract requires exactly 3 arguments\n";
                return nullptr;
            }

            // 获取高位索引i和低位索引j
            llvm::Value* i_val = arg_syms[0]->getValue();
            llvm::Value* j_val = arg_syms[1]->getValue();
            llvm::Value* bv = arg_syms[2]->getValue();

            // 检查i和j是否是常量
            if (auto* i_const = llvm::dyn_cast<llvm::ConstantInt>(i_val)) {
                if (auto* j_const = llvm::dyn_cast<llvm::ConstantInt>(j_val)) {
                    unsigned i = i_const->getZExtValue();
                    unsigned j = j_const->getZExtValue();

                    // 验证索引范围
                    unsigned bv_width = bv->getType()->getIntegerBitWidth();
                    if (i >= bv_width || j > i) {
                        llvm::errs() << "Error: invalid bit range [" << j << ":" << i
                                     << "] for bitvector of width " << bv_width << "\n";
                        return nullptr;
                    }

                    // 计算提取的位数
                    unsigned extract_width = i - j + 1;

                    // 1. 右移到位
                    llvm::Value* shifted = builder.CreateLShr(bv,
                                                              llvm::ConstantInt::get(bv->getType(), j));

                    // 2. 截断到所需宽度
                    llvm::Value* extracted = builder.CreateTrunc(shifted,
                                                                 llvm::IntegerType::get(*m_ctx, extract_width));

                    return extracted;
                }
            }

            // 如果不是常量索引，需要更复杂的处理
            llvm::errs() << "Error: non-constant indices not yet supported for extract\n";
            return nullptr;
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
        case Z3_OP_DISTINCT: {// add by yx
            Value* arg_size = ConstantInt::get(builder.getInt32Ty(), arg_syms.size());
            return builder.CreateCall(m_func_distinct, {arg_syms[0]->getValue(), arg_syms[1]->getValue(), arg_size});
        }
        case Z3_OP_BAND:
            return builder.CreateCall(m_func_band, {arg_syms[0]->getValue(),
                                                    arg_syms[1]->getValue()});
        case Z3_OP_BOR:{
            return builder.CreateCall(m_func_bor, {arg_syms[0]->getValue(),
                                                    arg_syms[1]->getValue()});
        }
        case Z3_OP_BXOR:{
            return builder.CreateCall(m_func_bxor, {arg_syms[0]->getValue(),
                                                   arg_syms[1]->getValue()});
        }
        case Z3_OP_FPA_TO_IEEE_BV:{
            // 将浮点数转换为它的位表示
            llvm::Value* fp_val = arg_syms[0]->getValue();

            if (fp_val->getType()->isDoubleTy()) {
                // 双精度浮点数 -> 64位位向量
                return builder.CreateBitCast(fp_val,
                                             llvm::Type::getInt64Ty(*m_ctx));
            }
            else if (fp_val->getType()->isFloatTy()) {
                // 单精度浮点数 -> 32位位向量
                return builder.CreateBitCast(fp_val,
                                             llvm::Type::getInt32Ty(*m_ctx));
            }
            llvm::errs() << "Error: unsupported floating-point type for to_ieee_bv\n";
            return nullptr;
        }
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
////    add by yx
//    llvm::Value* load_cov_value = builder.CreateLoad(cov);
//    llvm::Value* new_cov_value = builder.CreateFAdd(load_cov_value, m_const_one);
//    builder.CreateStore(new_cov_value, cov);
//    CallInst *call_res = nullptr;
//    if(arg_syms[0]->kind()==SymbolKind::kFP64Const || arg_syms[0]->kind()==SymbolKind::kFP64Var \
//        || arg_syms[1]->kind()==SymbolKind::kFP64Const || arg_syms[1]->kind()==SymbolKind::kFP64Var){
//        call_res = builder.CreateCall(m_func_fp64_dis, {arg_syms[0]->getValue(),
//                                                             arg_syms[1]->getValue()});
//    }
//    else if(arg_syms[0]->kind()==SymbolKind::kExpr){
//        unsigned sigd = Z3_fpa_get_sbits(arg_syms[0]->expr()->ctx(), arg_syms[0]->expr()->get_sort());
//        unsigned expo = Z3_fpa_get_ebits(arg_syms[0]->expr()->ctx(), arg_syms[0]->expr()->get_sort());
//        if(fpa_util::isFloat64(expo,sigd)){
//            call_res = builder.CreateCall(m_func_fp64_dis, {arg_syms[0]->getValue(),
//                                                            arg_syms[1]->getValue()});
//        }
//        else{
//            call_res = builder.CreateCall(m_func_fp32_dis, {arg_syms[0]->getValue(),
//                                                            arg_syms[1]->getValue()});
//        }
//    }
//    else if(arg_syms[1]->kind()==SymbolKind::kExpr){
//        unsigned sigd = Z3_fpa_get_sbits(arg_syms[1]->expr()->ctx(), arg_syms[1]->expr()->get_sort());
//        unsigned expo = Z3_fpa_get_ebits(arg_syms[1]->expr()->ctx(), arg_syms[1]->expr()->get_sort());
//        if(fpa_util::isFloat64(expo,sigd)){
//            call_res = builder.CreateCall(m_func_fp64_dis, {arg_syms[0]->getValue(),
//                                                            arg_syms[1]->getValue()});
//        }
//        else{
//            call_res = builder.CreateCall(m_func_fp32_dis, {arg_syms[0]->getValue(),
//                                                            arg_syms[1]->getValue()});
//        }
//    }
//    else{
//        call_res = builder.CreateCall(m_func_fp32_dis, {arg_syms[0]->getValue(),
//                                                        arg_syms[1]->getValue()});
//    }
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
////    add by yx
//    llvm::Value* load_cov_value = builder.CreateLoad(cov);
//    llvm::Value* new_cov_value = builder.CreateFAdd(load_cov_value, m_const_one);
//    builder.CreateStore(new_cov_value, cov);
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
// add by yx flagADD
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
                                                arg_syms[1]->getValue()); // Not equal
////            add by yx
//            llvm::BasicBlock* bb_true = llvm::BasicBlock::Create(*m_ctx, "", m_gofunc);
//            llvm::BasicBlock* bb_false = llvm::BasicBlock::Create(*m_ctx, "", m_gofunc);
//            llvm::BasicBlock* bb_cur = builder.GetInsertBlock();
//            builder.CreateCondBr(result, bb_true, bb_false);
//
//            builder.SetInsertPoint(bb_true);
//            llvm::Value* load_cov_value = builder.CreateLoad(cov);
//            llvm::Value* new_cov_value = builder.CreateFAdd(load_cov_value, m_const_one);
//            builder.CreateStore(new_cov_value, cov);
//            builder.CreateBr(bb_false);
//
//            builder.SetInsertPoint(bb_false);

            return builder.CreateSelect(result, m_const_zero, m_const_one);
        } else {
            return builder.CreateCall(m_func_fp64_eq_dis, {arg_syms[0]->getValue(),
                                                           arg_syms[1]->getValue()});
//            llvm::Value* dis = builder.CreateCall(m_func_fp64_eq_dis, {arg_syms[0]->getValue(),
//                                                        arg_syms[1]->getValue()});
////            add by yx
//            llvm::BasicBlock* bb_true = llvm::BasicBlock::Create(*m_ctx, "", m_gofunc);
//            llvm::BasicBlock* bb_false = llvm::BasicBlock::Create(*m_ctx, "", m_gofunc);
//            llvm::BasicBlock* bb_cur = builder.GetInsertBlock();
//            auto comp_res = builder.CreateFCmpOLE(dis, m_const_zero);
//            builder.CreateCondBr(comp_res, bb_true, bb_false);
//
//            builder.SetInsertPoint(bb_true);
//            llvm::Value* load_cov_value = builder.CreateLoad(cov);
//            llvm::Value* new_cov_value = builder.CreateFAdd(load_cov_value, m_const_one);
//            builder.CreateStore(new_cov_value, cov);
//            builder.CreateBr(bb_false);
//
//            builder.SetInsertPoint(bb_false);
//            return dis;
        }
    }
    // assuming Z3_OP_EQ
    if (expr_sym->isNegated()) {
        return builder.CreateCall(m_func_fp64_neq_dis,
                                  {arg_syms[0]->getValue(),
                                   arg_syms[1]->getValue()});
//        llvm::Value* dis = builder.CreateCall(m_func_fp64_neq_dis,
//                                  {arg_syms[0]->getValue(),
//                                   arg_syms[1]->getValue()});
////        add by yx
//        llvm::BasicBlock* bb_true = llvm::BasicBlock::Create(*m_ctx, "", m_gofunc);
//        llvm::BasicBlock* bb_false = llvm::BasicBlock::Create(*m_ctx, "", m_gofunc);
//        llvm::BasicBlock* bb_cur = builder.GetInsertBlock();
//        auto comp_res = builder.CreateFCmpOLE(dis, m_const_zero);
//        builder.CreateCondBr(comp_res, bb_true, bb_false);
//
//        builder.SetInsertPoint(bb_true);
//        llvm::Value* load_cov_value = builder.CreateLoad(cov);
//        llvm::Value* new_cov_value = builder.CreateFAdd(load_cov_value, m_const_one);
//        builder.CreateStore(new_cov_value, cov);
//        builder.CreateBr(bb_false);
//
//        builder.SetInsertPoint(bb_false);
//
//        return dis;
    } else {
        return builder.CreateCall(m_func_fp64_eq_dis,
                                  {arg_syms[0]->getValue(),
                                   arg_syms[1]->getValue()});
//        llvm::Value* dis = builder.CreateCall(m_func_fp64_eq_dis,
//                                  {arg_syms[0]->getValue(),
//                                   arg_syms[1]->getValue()});
//
////        add by yx
//        llvm::BasicBlock* bb_true = llvm::BasicBlock::Create(*m_ctx, "", m_gofunc);
//        llvm::BasicBlock* bb_false = llvm::BasicBlock::Create(*m_ctx, "", m_gofunc);
//        llvm::BasicBlock* bb_cur = builder.GetInsertBlock();
//        auto comp_res = builder.CreateFCmpOLE(dis, m_const_zero);
//        builder.CreateCondBr(comp_res, bb_true, bb_false);
//
//        builder.SetInsertPoint(bb_true);
//        llvm::Value* load_cov_value = builder.CreateLoad(cov);
//        llvm::Value* new_cov_value = builder.CreateFAdd(load_cov_value, m_const_one);
//        builder.CreateStore(new_cov_value, cov);
//        builder.CreateBr(bb_false);
//
//        builder.SetInsertPoint(bb_false);
//
//        return dis;
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
    double (*func_ptr_32_dis)(float, float) = fp32_dis;
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
    double (*func_ptr_distinct)(double, double, int) = fp64_distinct;//add by yx
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
    engine->addGlobalMapping(this->m_func_fp32_dis, (void *)func_ptr_32_dis);
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
    engine->addGlobalMapping(this->m_func_distinct, (void *)func_ptr_distinct);//add by yx
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
