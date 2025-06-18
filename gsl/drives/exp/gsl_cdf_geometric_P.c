//
// Created by liukunlin on 2021/12/17.
//
#include "klee/klee.h"
#include "gsl/gsl_cdf.h"
int main() {
    unsigned int k;
    double p;
    klee_make_symbolic(&k, sizeof(k), "k");
    klee_make_symbolic(&p, sizeof(p), "p");
    gsl_cdf_geometric_P (k, p);
}

