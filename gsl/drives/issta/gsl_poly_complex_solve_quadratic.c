//
// Created by liukunlin on 2022/1/16.
//

#include "klee/klee.h"
#include "gsl/gsl_poly.h"

int main(){
    double a,b,c;
    gsl_complex *z0, *z1;
    klee_make_symbolic(&a,sizeof(a),"a");
    klee_make_symbolic(&b,sizeof(b),"b");
    klee_make_symbolic(&c,sizeof(c),"c");
    gsl_poly_complex_solve_quadratic(a,b,c,z0,z1);
}