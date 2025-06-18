//
// Created by liukunlin on 2021/8/29.
//
#include "klee/klee.h"
#include "gsl_sf_bessel.h"
int main()
{
    double x;
    int n;
    klee_make_symbolic(&x,sizeof(x),"x");
    klee_make_symbolic(&n,sizeof(n),"n");
    gsl_sf_result result;
    gsl_sf_bessel_In_scaled_e(n,x, &result);
}
