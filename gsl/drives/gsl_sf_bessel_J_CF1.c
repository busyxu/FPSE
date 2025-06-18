//
// Created by liukunlin on 2021/8/29.
//
#include "klee/klee.h"
#include "bessel.h"
int main()
{
    double nu,x;
    double ratio;
    klee_make_symbolic(&nu,sizeof(nu),"nu");
    klee_make_symbolic(&x,sizeof(x),"x");
    double sgn;
    gsl_sf_bessel_J_CF1(nu, x,
    &ratio, &sgn);
}

