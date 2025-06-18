//
// Created by liukunlin on 2021/8/29.
//
#include "klee/klee.h"
#include "gsl/gsl_cdf.h"
int main()
{
    double P,a,b;
    klee_make_symbolic(&P, sizeof(P),"P");
    klee_make_symbolic(&a, sizeof(a),"a");
    klee_make_symbolic(&b, sizeof(b),"b");
    gsl_cdf_beta_Pinv (P, a, b);
}