//
// Created by liukunlin on 2021/12/17.
//

#include "klee/klee.h"
#include "gsl/gsl_cdf.h"
int main() {
    double x, a;
    klee_make_symbolic(&x, sizeof(x), "x");
    klee_make_symbolic(&a, sizeof(a), "a");
    gsl_cdf_laplace_P(x, a);
}