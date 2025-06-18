//
// Created by liukunlin on 2021/8/31.
//
#include "klee/klee.h"
#include "gsl/gsl_cdf.h"
int main()
{
    double x,mu;
    klee_make_symbolic(&x, sizeof(x),"x");
    klee_make_symbolic(&mu, sizeof(mu),"mu");
    gsl_cdf_exponential_P(x, mu);
}

