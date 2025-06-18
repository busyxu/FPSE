//
// Created by liukunlin on 2021/8/29.
//
#include "klee/klee.h"
#include "gsl/gsl_sf_airy.h"
#include "gsl/gsl_sf_result.h"

int main()
{
    double x;
    klee_make_symbolic(&x,sizeof(x),"x");
    gsl_mode_t mode;
    klee_make_symbolic(&mode,sizeof(mode),"mode");
    gsl_sf_result result;
    gsl_sf_airy_Bi_deriv_e(x,mode,&result);
}
