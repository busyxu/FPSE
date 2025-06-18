/******************************************************************************
 * Top contributors (to current version):
 *   Morgan Deters, Andrew Reynolds, Andres Noetzli
 *
 * This file is part of the cvc5 project.
 *
 * Copyright (c) 2009-2022 by the authors listed in the file AUTHORS
 * in the top-level source directory and their institutional affiliations.
 * All rights reserved.  See the file COPYING in the top-level source
 * directory for licensing information.
 * ****************************************************************************
 *
 * A simple test of multiple SmtEngines.
 */

#include <cvc5/cvc5.h>

#include <iostream>
#include <sstream>
#undef FLT_MAX
#undef DBL_MAX
#undef LDBL_MAX
#define FLT_MAX		__FLT_MAX__
#define DBL_MAX		__DBL_MAX__
#define LDBL_MAX	__LDBL_MAX__

using namespace cvc5;
using namespace std;

int main() {

  Solver solver;

  //! [docs-cpp-quickstart-2 start]
  solver.setOption("produce-models", "true");
  //! [docs-cpp-quickstart-2 end]

  // Set the logic
  //! [docs-cpp-quickstart-3 start]
  solver.setLogic("ALL");
  //! [docs-cpp-quickstart-3 end]

  //! [docs-cpp-quickstart-4 start]
  Sort realSort = solver.getRealSort();
  Sort intSort = solver.getIntegerSort();
  //! [docs-cpp-quickstart-4 end]

  //! add by yx
  Term nu1_cvc5 = solver.mkConst(realSort, "nu1_cvc5");
  Term nu2_cvc5 = solver.mkConst(realSort, "nu2_cvc5");
  Term x_cvc5 = solver.mkConst(realSort, "x_cvc5");

  //! (> (abs (- 1.0 (^ (/ nu2_cvc5 x_cvc5) nu1_cvc5))) DBL_MAX)
  Term oneyx = solver.mkReal(1);
  Term base = solver.mkTerm(DIVISION, {nu2_cvc5, x_cvc5});
  Term pow = solver.mkTerm(POW, {base, nu1_cvc5});
  Term sub = solver.mkTerm(SUB, {oneyx, pow});
  Term abs = solver.mkTerm(ABS, {sub});
//  Term Dmax = solver.mkReal(std::to_string(DBL_MAX));
  Term zero = solver.mkReal(0);
  Term Gt = solver.mkTerm(GT, {abs, zero});

  solver.assertFormula(Gt);

  //! [docs-cpp-quickstart-7 start]
  Result r1 = solver.checkSat();
  //! [docs-cpp-quickstart-7 end]

  //! [docs-cpp-quickstart-8 start]
  std::cout << "expected: sat" << std::endl;
  std::cout << "result: " << r1 << std::endl;
  //! [docs-cpp-quickstart-8 end]

  //! [docs-cpp-quickstart-9 start]
  Term nu1Val = solver.getValue(nu1_cvc5);
  Term nu2Val = solver.getValue(nu2_cvc5);
  Term xVal = solver.getValue(x_cvc5);
  //! [docs-cpp-quickstart-9 end]

  // We can now obtain the string representations of the values.
  //! [docs-cpp-quickstart-11 start]
  std::string nu1Str = nu1Val.getRealValue();
  std::string nu2Str = nu2Val.getRealValue();
  std::string xStr = xVal.getRealValue();

  std::cout << "value for nu1: " << nu1Str << std::endl;
  std::cout << "value for nu2: " << nu2Str << std::endl;
  std::cout << "value for x: " << xStr << std::endl;
  //! [docs-cpp-quickstart-11 end]1;
}

