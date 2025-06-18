//
// Created by liukunlin on 2021/8/19.
//
#include "klee/klee.h"
#include "gsl/gsl_interp.h"
int main()
{
    size_t size=4;
//    klee_make_symbolic(&size, sizeof(size),"size");
//    klee_assume(size>=4);
    gsl_interp * interp = gsl_interp_alloc(gsl_interp_linear,size);
    double xa[4];
    double ya[4];
    klee_make_symbolic(xa, sizeof(double)*4,"xa");
    klee_make_symbolic(ya, sizeof(double)*4,"ya");
    double x;
    klee_make_symbolic(&x, sizeof(x),"x");
    double dydx;
    gsl_interp_accel * a = gsl_interp_accel_alloc();
    gsl_interp_eval_deriv_e (interp,
                             xa, ya, x,
                             a,
                             &dydx);
}
