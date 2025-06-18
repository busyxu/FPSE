//
// Created by liukunlin on 2022/1/16.
//

#include "klee/klee.h"
#include "gsl/gsl_poly.h"

int main(){
    double *a;
    size_t n = 8;
    gsl_poly_complex_workspace *w = gsl_poly_complex_workspace_alloc (n);
    gsl_complex_packed_ptr z;
    klee_make_symbolic(a,sizeof(double)*n,"a");
    klee_make_symbolic(z,sizeof(double)*n*2,"z");
    gsl_poly_complex_solve(a,n,w,z);
}