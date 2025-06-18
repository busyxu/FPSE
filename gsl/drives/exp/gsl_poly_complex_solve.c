//
// Created by liukunlin on 2021/8/31.
//
#include "klee/klee.h"
#include "gsl/gsl_poly.h"

void symbolizeVector(double *v, size_t len) {
    char name[3] = {'a', 'a', 0};
    for (int i = 0; i < len; i++) {
        double *p = malloc(sizeof(double));
        name[1] = '0' + i;
        klee_make_symbolic(p, sizeof(double), name);
        v[i] = *p;
    }
}
int main()
{

    size_t n=6;
    double a[n];
    symbolizeVector(a,n);
    gsl_poly_complex_workspace * w = gsl_poly_complex_workspace_alloc(n);
    double t[n];
    gsl_complex_packed_ptr z = t;
    gsl_poly_complex_solve (a, n,
            w,z);
}
