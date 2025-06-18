//
// Created by liukunlin on 2021/8/28.
//
#include "klee/klee.h"
#include "gsl/gsl_fit.h"
int main()
{
    size_t xstride=1;
    double *x,*y;
    size_t ystride=1;
    size_t n=3;
    x = (double *)malloc(sizeof(double)*n*xstride);
    y = (double *)malloc(sizeof(double)*n*ystride);
    double c0,c1,cov_00, cov_01, cov_11, sumsq;
    klee_make_symbolic(x,sizeof(x),"x");
    klee_make_symbolic(y,sizeof(y),"y");
//    klee_make_symbolic(c0,sizeof(c0),"c0");
//    klee_make_symbolic(c1,sizeof(c1),"c1");
//    klee_make_symbolic(cov_00,sizeof(cov_00),"cov_00");
//    klee_make_symbolic(cov_01,sizeof(cov_01),"cov_01");
//    klee_make_symbolic(cov_11,sizeof(cov_11),"cov_11");
//    klee_make_symbolic(sumsq,sizeof(sumsq),"sumsq");


    gsl_fit_linear (x, xstride, y, ystride, n,&c0, &c1,&cov_00, &cov_01, &cov_11, &sumsq);
}