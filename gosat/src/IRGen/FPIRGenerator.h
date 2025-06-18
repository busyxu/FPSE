//===------------------------------------------------------------*- C++ -*-===//
//
// This file is distributed under MIT License. See LICENSE.txt for details.
//
//===----------------------------------------------------------------------===//
//
// Copyright (c) 2017 University of Kaiserslautern.
//

#pragma once

#include "CodeGen/CodeGen.h"
#include "z3++.h"
#include "llvm/IR/Module.h"
#include <llvm/IR/IRBuilder.h>
#include <unordered_map>
#include <llvm/ExecutionEngine/ExecutionEngine.h>

namespace gosat {

class IRSymbol : public Symbol {
public:
    IRSymbol() = delete;

    IRSymbol(SymbolKind kind, const z3::expr expr, llvm::Value* value,
             unsigned id = 0)
            : Symbol{kind, expr}, m_value{value}, m_id{id}
    {}

    IRSymbol(const IRSymbol&) = default;

    IRSymbol& operator=(const IRSymbol&) = default;

    IRSymbol& operator=(IRSymbol&&) = delete;

    virtual ~IRSymbol() = default;

    void setValue(llvm::Value* value) noexcept
    {
        m_value = value;
    }

    llvm::Value* getValue() const noexcept
    {
        return m_value;
    }

    unsigned id() const noexcept
    {
        return m_id;
    }

private:
    llvm::Value* m_value;
    unsigned m_id;
};

using SymMapType = std::unordered_map<unsigned, IRSymbol>;

class FPIRGenerator {
public:
    FPIRGenerator() = delete;

    explicit FPIRGenerator(llvm::LLVMContext* context, llvm::Module* module);

    virtual ~FPIRGenerator() = default;

    FPIRGenerator(const FPIRGenerator&) = default;

    FPIRGenerator& operator=(const FPIRGenerator&) = default;

    FPIRGenerator& operator=(FPIRGenerator&&) = default;

    llvm::Function* genFunction(const z3::expr& expr) noexcept;

    llvm::Function* getDistanceFunction() const noexcept;

    unsigned getVarCount() const noexcept;

    const std::vector<IRSymbol*>& getVars() const noexcept;

    const std::vector<std::pair<IRSymbol*, const IRSymbol*>>&
    getVarsFPAWrapped() const noexcept;

    void addGlobalFunctionMappings(llvm::ExecutionEngine *engine);

    bool isFoundUnsupportedSMTExpr() noexcept;

private:
    const IRSymbol* genFuncRecursive
            (llvm::IRBuilder<>& builder, const z3::expr expr,
             bool is_negated) noexcept;

    const IRSymbol*
    genNumeralIR(llvm::IRBuilder<>& builder, const z3::expr& expr) noexcept;

    llvm::Value* genExprIR
            (llvm::IRBuilder<>& builder, const IRSymbol* expr_sym,
             std::vector<const IRSymbol*>& arg_syms) noexcept;

    llvm::Value* genBinArgCmpIR
            (llvm::IRBuilder<>& builder,
             std::vector<const IRSymbol*>& arg_syms,
             llvm::Value* comp_result) noexcept;

//    [add by yx]
//    llvm::Value* genBinArgCmpIR3
//            (llvm::IRBuilder<>& builder,
//             std::vector<const IRSymbol*>& arg_syms,
//             llvm::Value* comp_result,
//             Z3_decl_kind kind) noexcept;

    llvm::Value* genBinArgCmpIR2
            (llvm::IRBuilder<>& builder,
             std::vector<const IRSymbol*>& arg_syms,
             llvm::Value* comp_result) noexcept;

    llvm::Value* genMultiArgAddIR
            (llvm::IRBuilder<>& builder,
             std::vector<const IRSymbol*>& arg_syms) noexcept;

    llvm::Value* genMultiArgMulIR
            (llvm::IRBuilder<>& builder,
             std::vector<const IRSymbol*>& arg_syms) noexcept;

    llvm::Value *genEqualityIR
            (llvm::IRBuilder<> &builder, const IRSymbol *expr_sym,
             std::vector<const IRSymbol *> &arg_syms) noexcept;

    // add by zgf
    llvm::Value *genFPCheckIR
            (llvm::IRBuilder<> &builder, std::vector<const IRSymbol*>& arg_syms,
             llvm::Value* comp_result,int opcode, int mode) noexcept;

    std::pair<IRSymbol*, bool> insertSymbol
            (const SymbolKind kind, const z3::expr expr, llvm::Value* value,
             unsigned id = 0) noexcept;

    SymMapType::const_iterator
    findSymbol(const SymbolKind kind, const z3::expr* expr) const noexcept;

private:
    bool m_has_invalid_fp_const;
    bool m_found_unsupported_smt_expr;
    llvm::Function* m_gofunc;
    llvm::Function* m_func_fp64_dis;
//    [add by yx]
    llvm::Function* m_func_fp64_gt_dis;
    llvm::Function* m_func_fp64_lt_dis;
    llvm::Function* m_func_fp64_ge_dis;
    llvm::Function* m_func_fp64_le_dis;
    llvm::Function* m_func_fp64_overflow_dis;
    llvm::Function* m_func_fp64_eq_dis;
    llvm::Function* m_func_fp64_neq_dis;
    llvm::Function* m_func_isnan;
    // add by zgf
    llvm::Function* m_func_isinf;
    llvm::Function* m_func_ite;
    llvm::Function* m_func_band;
    llvm::Function* m_func_bor;
    llvm::Function* m_func_bxor;
    llvm::Function* m_func_tan;
    llvm::Function* m_func_asin;
    llvm::Function* m_func_acos;
    llvm::Function* m_func_atan;
    llvm::Function* m_func_sinh;
    llvm::Function* m_func_cosh;
    llvm::Function* m_func_tanh;
    llvm::Function* m_func_pow;
    llvm::Function* m_func_atan2;
    llvm::Function* m_func_fmin;
    llvm::Function* m_func_fmax;

    llvm::Function* m_func_fpcheck_dis;
    llvm::Function* m_func_fpcheck_fadd_overflow;
    llvm::Function* m_func_fpcheck_fsub_overflow;
    llvm::Function* m_func_fpcheck_fmul_overflow;
    llvm::Function* m_func_fpcheck_fdiv_overflow;
    llvm::Function* m_func_fpcheck_fadd_underflow;
    llvm::Function* m_func_fpcheck_fsub_underflow;
    llvm::Function* m_func_fpcheck_fmul_underflow;
    llvm::Function* m_func_fpcheck_fdiv_underflow;
    llvm::Function* m_func_fpcheck_fdiv_invalid;
    llvm::Function* m_func_fpcheck_fdiv_zero;
//add by yx
    llvm::Function* m_func_fpcheck_finvalid_sqrt;
    llvm::Function* m_func_fpcheck_finvalid_log;
    llvm::Function* m_func_fpcheck_finvalid_pow;
    llvm::Function* m_func_fpcheck_fadd_accuracy;
    llvm::Function* m_func_fpcheck_fsub_accuracy;
    llvm::Function* m_func_fpcheck_fmul_accuracy;
    llvm::Function* m_func_fpcheck_fdiv_accuracy;

    llvm::Constant* m_const_zero;
    llvm::Constant* m_const_one;
    llvm::LLVMContext* m_ctx;
    llvm::Module* m_mod;
    llvm::MDNode* m_tbaa_node;
    std::vector<IRSymbol*> m_var_sym_vec;
    std::vector<std::pair<IRSymbol*, const IRSymbol*>> m_var_sym_fpa_vec;
    SymMapType m_expr_sym_map;
};
}
