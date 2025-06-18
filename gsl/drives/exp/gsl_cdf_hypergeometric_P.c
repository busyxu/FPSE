//
// Created by liukunlin on 2021/12/17.
//

#include "klee/klee.h"
#include "gsl/gsl_cdf.h"
int main() {
    unsigned int x, a, b, t;
    klee_make_symbolic(&x, sizeof(x), "x");
    klee_make_symbolic(&a, sizeof(a), "a");
    klee_make_symbolic(&b, sizeof(b), "b");
    klee_make_symbolic(&t, sizeof(t), "t");
    gsl_cdf_hypergeometric_P(x, a, b, t);
}