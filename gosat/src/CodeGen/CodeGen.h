//===------------------------------------------------------------*- C++ -*-===//
//
// This file is distributed under MIT License. See LICENSE.txt for details.
//
//===----------------------------------------------------------------------===//
//
// Copyright (c) 2017 University of Kaiserslautern.
//

#pragma once

#include "z3++.h"

namespace gosat {
/**
 * /brief String constants used for code generation
 */
class CodeGenStr {
public:
    static const std::string kFunName;
    static const std::string kFunInput;
    static const std::string kFunDis;
//    [add by yx]
    static const std::string kFunGtDis;
    static const std::string kFunLtDis;
    static const std::string kFunGeDis;
    static const std::string kFunLeDis;
    static const std::string kFunOverflowDis;

    static const std::string kFunEqDis;
    static const std::string kFunNEqDis;
    static const std::string kFunIsNan;
    // add by zgf
    static const std::string kFunIsInf;
    static const std::string kFunIte;
    static const std::string kFunBand;
    static const std::string kFunBor;
    static const std::string kFunBxor;
    static const std::string kFunTan;
    static const std::string kFunASin;
    static const std::string kFunACos;
    static const std::string kFunATan;
    static const std::string kFunSinh;
    static const std::string kFunCosh;
    static const std::string kFunTanh;
    static const std::string kFunPow;
    static const std::string kFunATan2;
    static const std::string kFunFMin;
    static const std::string kFunFMax;

    static const std::string kFunFPDis;
    static const std::string kFunFAddOverflow;
    static const std::string kFunFSubOverflow;
    static const std::string kFunFMulOverflow;
    static const std::string kFunFDivOverflow;
    static const std::string kFunFAddUnderflow;
    static const std::string kFunFSubUnderflow;
    static const std::string kFunFMulUnderflow;
    static const std::string kFunFDivUnderflow;
    static const std::string kFunFDivInvalid;
    static const std::string kFunFDivZero;
//add by yx
    static const std::string kFunFInvalidSqrt;
    static const std::string kFunFInvalidLog;
    static const std::string kFunFInvalidPow;
    static const std::string kFunFAddAccuracy;
    static const std::string kFunFSubAccuracy;
    static const std::string kFunFMulAccuracy;
    static const std::string kFunFDivAccuracy;
};

enum class SymbolKind : unsigned {
    kExpr = 0,
    kNegatedExpr = 1,
    kFP32Const = 2,
    kFP64Const = 4,
    kFP32Var = 8,
    kFP64Var = 16,
    kUnknown = 32
};

enum class FPVarKind : unsigned {
    kUnknown,
    kFP32,
    kFP64,
};

class Symbol {
public:
    Symbol() = delete;

    virtual ~Symbol() = default;

    Symbol(const Symbol&) = default;

    Symbol& operator=(const Symbol&) = default;

    Symbol& operator=(Symbol&&) = default;

    explicit Symbol(SymbolKind kind, const z3::expr expr);

    SymbolKind kind() const noexcept;

    const z3::expr* expr() const noexcept;

    const char* name() const noexcept;

    bool isNegated() const noexcept;

private:
    const SymbolKind m_kind;
    const z3::expr m_expr;
    const std::string m_name;
};
}
