//
// Created by liukunlin on 2021/8/29.
//
#include "klee/klee.h"
#include "gsl/gsl_sf_bessel.h"
#include "bessel.h"
int main()
{
    double mu,x;
    gsl_sf_result Jmu, Jmup1,Ymu, Ymup1;
    klee_make_symbolic(&mu, sizeof(mu),"nu");
    klee_make_symbolic(&x, sizeof(x),"x");

    gsl_sf_bessel_Knu_scaled_asymp_unif_e(mu,x,
                                          &Jmu, &Jmup1,&mu, &Ymup1
    );
}
