//
// Created by liukunlin on 2021/12/17.
//

#include "klee/klee.h"
#include "gsl/gsl_cdf.h"
int main() {
    double x, a, b;
    klee_make_symbolic(&x, sizeof(x), "x");
    klee_make_symbolic(&a, sizeof(a), "a");
    klee_make_symbolic(&b, sizeof(b), "b");
    gsl_cdf_pareto_Q(x, a, b);
}