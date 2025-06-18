//
// Created by liukunlin on 2021/8/29.
//
#include "klee/klee.h"
#include "gsl_sf_bessel.h"
int main()
{
    double x;
    double nu;
    klee_make_symbolic(&x,sizeof(x),"x");
    klee_make_symbolic(&nu,sizeof(nu),"nu");
    gsl_sf_result result;
    gsl_sf_bessel_Inu_scaled_e(nu,x, &result);
}
