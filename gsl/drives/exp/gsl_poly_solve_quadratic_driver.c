//
// Created by liukunlin on 2021/8/19.
//
#include "klee/klee.h"
#include "gsl/gsl_poly.h"

/// folder: poly
/// this poly functions has a complex condition that may result in very few paths due to time out
/// not very good candidate


int main()
{
    double a,b,c;
    //double x0,x1;
    klee_make_symbolic(&a, sizeof(double), "a");
    klee_make_symbolic(&b, sizeof(double), "b");
    klee_make_symbolic(&c, sizeof(double), "c");
    //gsl_poly_solve_quadratic(a, b, c, &x0, &x1);
    gsl_complex x0, x1;
    gsl_poly_complex_solve_quadratic(a, b, c, &x0, &x1);
}
