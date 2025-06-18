//
// Created by liukunlin on 2021/8/29.
//
#include "klee/klee.h"
#include "gsl/gsl_sf_bessel.h"
#include "bessel.h"
int main()
{
    double nu,x;
    gsl_sf_result result;
    klee_make_symbolic(&nu, sizeof(nu),"nu");
    klee_make_symbolic(&x, sizeof(x),"x");

    gsl_sf_bessel_Ynu_asympx_e(nu,x,
                               &result
    );
}
