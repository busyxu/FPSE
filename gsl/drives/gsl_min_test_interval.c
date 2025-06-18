//
// Created by liukunlin on 2021/8/28.
//
#include "klee/klee.h"
#include "gsl/gsl_min.h"

int main()
{
    double x_lower, x_upper,epsabs, epsrel;
    klee_make_symbolic(&x_lower,sizeof(x_lower),"x_lower");
    klee_make_symbolic(&epsabs,sizeof(epsabs),"epsabs");
    klee_make_symbolic(&x_upper,sizeof(x_upper),"x_upper");
    klee_make_symbolic(&epsrel,sizeof(epsrel),"epsrel");
    gsl_min_test_interval (x_lower, x_upper,epsabs, epsrel);
}