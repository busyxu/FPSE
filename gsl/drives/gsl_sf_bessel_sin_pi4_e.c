//
// Created by liukunlin on 2021/8/29.
//
#include "klee/klee.h"
#include "bessel.h"
int main()
{
    double y;
    klee_make_symbolic(&y,sizeof(y),"y");
    double eps;
    klee_make_symbolic(&eps,sizeof(eps),"eps");
    gsl_sf_result result;
    gsl_sf_bessel_sin_pi4_e(y, eps, &result);
}
