//
// Created by liukunlin on 2021/8/19.
//
#include "klee/klee.h"
#include "gsl/gsl_interp.h"
int main()
{
    size_t size=3;
    //    klee_make_symbolic(&size, sizeof(size),"size");
    //    klee_assume(size>=4);
    gsl_interp * interp = gsl_interp_alloc(gsl_interp_linear,size);
    double xa[4];
    double ya[4];
    klee_make_symbolic(xa, sizeof(double)*4,"xa");
    klee_make_symbolic(ya, sizeof(double)*4,"ya");
    double a,b;
    klee_make_symbolic(&a, sizeof(a),"a");
    klee_make_symbolic(&b, sizeof(b),"b");
//    klee_assume(xa[0]<xa[1]);
//    klee_assume(xa[1]<xa[2]);
//    klee_assume(xa[2]<xa[3]);
    gsl_interp_init(interp,xa,ya,size);
//    klee_assume(a <= b);
//    klee_assume(a >= interp->xmin);
//    klee_assume(b <= interp->xmax);
    //    double dydx;
    gsl_interp_accel * acc = gsl_interp_accel_alloc();
    gsl_interp_eval_integ (interp,
                           xa, ya,
                           a, b,
                           acc);
}
