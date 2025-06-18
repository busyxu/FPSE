//
// Created by liukunlin on 2021/12/17.
//

#include "klee/klee.h"
#include "gsl/gsl_cdf.h"
int main() {
    unsigned int x;
    double a;
    klee_make_symbolic(&x, sizeof(x), "x");
    klee_make_symbolic(&a, sizeof(a), "a");
    gsl_cdf_poisson_Q(x, a);
}