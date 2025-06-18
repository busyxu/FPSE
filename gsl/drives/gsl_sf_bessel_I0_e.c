//
// Created by liukunlin on 2021/8/29.
//
#include "klee/klee.h"
#include "gsl_sf_bessel.h"
int main()
{
    double x;
    klee_make_symbolic(&x,sizeof(x),"x");
    gsl_sf_result result;
    gsl_sf_bessel_I0_e(x, &result);
}
