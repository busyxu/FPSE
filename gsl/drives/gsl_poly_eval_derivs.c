//
// Created by liukunlin on 2021/8/28.
//
#include "klee/klee.h"
#include <gsl/gsl_poly.h>
int main()
{
    size_t lenc=3;
    double c[lenc];
    size_t lenres=3;
    double res[lenres];
    double x;
    klee_make_symbolic(&x,sizeof(x),"x");
    klee_make_symbolic(c,sizeof(c),"c");
    klee_make_symbolic(res,sizeof(res),"res");
    gsl_poly_eval_derivs (c, lenc, x, res, lenres);
}