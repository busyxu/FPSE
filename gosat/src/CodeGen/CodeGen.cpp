//===------------------------------------------------------------*- C++ -*-===//
//
// This file is distributed under MIT License. See LICENSE.txt for details.
//
//===----------------------------------------------------------------------===//
//
// Copyright (c) 2017 University of Kaiserslautern.
//

#include "CodeGen.h"

namespace gosat {

const std::string CodeGenStr::kFunName = "gofunc";
const std::string CodeGenStr::kFunInput = "x";
const std::string CodeGenStr::kFunDis = "fp64_dis";
//[add by yx]
const std::string CodeGenStr::kFunGtDis = "fp64_gt_dis";
const std::string CodeGenStr::kFunLtDis = "fp64_lt_dis";
const std::string CodeGenStr::kFunGeDis = "fp64_ge_dis";
const std::string CodeGenStr::kFunLeDis = "fp64_le_dis";
const std::string CodeGenStr::kFunOverflowDis = "fp64_overflow_dis";

const std::string CodeGenStr::kFunEqDis = "fp64_eq_dis";
const std::string CodeGenStr::kFunNEqDis = "fp64_neq_dis";
const std::string CodeGenStr::kFunIsNan = "fp64_isnan";
// add by zgf
const std::string CodeGenStr::kFunIsInf = "fp64_isinf";
const std::string CodeGenStr::kFunIte = "fp64_ite";
const std::string CodeGenStr::kFunBand = "fp64_band";
const std::string CodeGenStr::kFunBor = "fp64_bor";
const std::string CodeGenStr::kFunBxor = "fp64_bxor";
const std::string CodeGenStr::kFunTan = "fp64_tan";
const std::string CodeGenStr::kFunASin = "fp64_asin";
const std::string CodeGenStr::kFunACos = "fp64_acos";
const std::string CodeGenStr::kFunATan = "fp64_atan";
const std::string CodeGenStr::kFunSinh = "fp64_sinh";
const std::string CodeGenStr::kFunCosh = "fp64_cosh";
const std::string CodeGenStr::kFunTanh = "fp64_tanh";
const std::string CodeGenStr::kFunPow = "fp64_pow";
const std::string CodeGenStr::kFunATan2 = "fp64_atan2";
const std::string CodeGenStr::kFunFMin = "fp64_fmin";
const std::string CodeGenStr::kFunFMax = "fp64_fmax";

const std::string CodeGenStr::kFunFPDis = "fpcheck_dis";
const std::string CodeGenStr::kFunFAddOverflow = "FPCHECK_FADD_OVERFLOW";
const std::string CodeGenStr::kFunFSubOverflow = "FPCHECK_FSUB_OVERFLOW";
const std::string CodeGenStr::kFunFMulOverflow = "FPCHECK_FMUL_OVERFLOW";
const std::string CodeGenStr::kFunFDivOverflow = "FPCHECK_FDIV_OVERFLOW";
const std::string CodeGenStr::kFunFAddUnderflow = "FPCHECK_FADD_UNDERFLOW";
const std::string CodeGenStr::kFunFSubUnderflow = "FPCHECK_FSUB_UNDERFLOW";
const std::string CodeGenStr::kFunFMulUnderflow = "FPCHECK_FMUL_UNDERFLOW";
const std::string CodeGenStr::kFunFDivUnderflow = "FPCHECK_FDIV_UNDERFLOW";
const std::string CodeGenStr::kFunFDivInvalid = "FPCHECK_FDIV_INVALID";
const std::string CodeGenStr::kFunFDivZero = "FPCHECK_FDIV_ZERO";
//add by yx
const std::string CodeGenStr::kFunFInvalidSqrt = "FPCHECK_FINVALID_SQRT";
const std::string CodeGenStr::kFunFInvalidLog = "FPCHECK_FINVALID_LOG";
const std::string CodeGenStr::kFunFInvalidPow = "FPCHECK_FINVALID_POW";

const std::string CodeGenStr::kFunFAddAccuracy = "FPCHECK_FADD_ACCURACY";
const std::string CodeGenStr::kFunFSubAccuracy = "FPCHECK_FSUB_ACCURACY";
const std::string CodeGenStr::kFunFMulAccuracy = "FPCHECK_FMUL_ACCURACY";
const std::string CodeGenStr::kFunFDivAccuracy = "FPCHECK_FDIV_ACCURACY";

Symbol::Symbol(SymbolKind kind, const z3::expr expr) :
        m_kind{kind},
        m_expr{expr},
        m_name{"expr_" + std::to_string(expr.hash())
               + (kind == SymbolKind::kNegatedExpr ? "n"
                                                   : "")}
{}

SymbolKind Symbol::kind() const noexcept
{
    return m_kind;
}

const z3::expr* Symbol::expr() const noexcept
{
    return &m_expr;
}

const char* Symbol::name() const noexcept
{
    return m_name.c_str();
}

bool Symbol::isNegated() const noexcept
{
    return m_kind == SymbolKind::kNegatedExpr;
}
}
