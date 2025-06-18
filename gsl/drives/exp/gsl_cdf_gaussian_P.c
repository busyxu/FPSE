//
// Created by liukunlin on 2021/12/17.
//
#include "klee/klee.h"
#include "gsl/gsl_cdf.h"

int main(){
    double x, sigma;
    klee_make_symbolic(&x, sizeof(x), "x");
    klee_make_symbolic(&sigma, sizeof(x), "sigma");
    gsl_cdf_gaussian_P(x,sigma);
}