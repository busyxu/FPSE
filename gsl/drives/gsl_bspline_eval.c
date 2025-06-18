//
// Created by liukunlin on 2021/8/29.
//
#include "klee/klee.h"
#include "gsl/gsl_bspline.h"
int main()
{
    size_t k = 4;
    size_t n = 3;
    double a,b;
    klee_make_symbolic(&a, sizeof(a),"a");
    klee_make_symbolic(&b, sizeof(b),"b");
    gsl_bspline_workspace * w = gsl_bspline_alloc(k,n);
    gsl_bspline_knots_uniform(a,b,w);
    size_t ncoeffs = gsl_bspline_ncoeffs(w);
    gsl_vector *B  = gsl_vector_alloc(ncoeffs);
    double x;
    klee_make_symbolic(&x,sizeof(x),"x");
    gsl_bspline_eval(x,B,w);


}