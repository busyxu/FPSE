//
// Created by liukunlin on 2021/8/29.
//
#include "klee/klee.h"
#include "bessel.h"
int main()
{
    double nu,x;
    double P,Q;
    klee_make_symbolic(&nu,sizeof(nu),"nu");
    klee_make_symbolic(&x,sizeof(x),"x");
    gsl_sf_bessel_JY_steed_CF2(nu, x,
                            &P,&Q);
}
