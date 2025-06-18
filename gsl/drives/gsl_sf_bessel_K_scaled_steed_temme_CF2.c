//
// Created by liukunlin on 2021/8/29.
//
#include "klee/klee.h"
#include "bessel.h"
int main()
{
    double nu,x;
    double K_nu, K_nup1;
    double Kp_nu;
    klee_make_symbolic(&nu,sizeof(nu),"nu");
    klee_make_symbolic(&x,sizeof(x),"x");
    gsl_sf_bessel_K_scaled_steed_temme_CF2(nu, x,
                                           &K_nu, &K_nup1,&Kp_nu);
}
