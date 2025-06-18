//
// Created by liukunlin on 2021/8/29.
//
#include "klee/klee.h"
#include "gsl/gsl_sf_bessel.h"
#include "bessel.h"
int main()
{
    double nu,x;
    int sign,kmax;
    double threshold;
    gsl_sf_result result;
    klee_make_symbolic(&nu, sizeof(nu),"nu");
    klee_make_symbolic(&sign, sizeof(sign),"sign");
    klee_make_symbolic(&kmax, sizeof(kmax),"kmax");
    klee_make_symbolic(&x, sizeof(x),"x");
    klee_make_symbolic(&threshold, sizeof(threshold),"threshold");

    gsl_sf_bessel_IJ_taylor_e(nu,x,sign,kmax,threshold,
            &result
    );
}