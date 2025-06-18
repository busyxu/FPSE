//
// Created by liukunlin on 2022/1/17.
//
#include "klee/klee.h"
#include "gsl/gsl_integration.h"


int main(){
    double omega, L;
    klee_make_symbolic(&omega, sizeof(omega),"omega");
    klee_make_symbolic(&L, sizeof(L),"L");
    size_t n = 1000;
    gsl_integration_qawo_table * t = gsl_integration_qawo_table_alloc(omega,L,GSL_INTEG_COSINE,n);
    gsl_integration_qawo_table_set(t,omega,L,GSL_INTEG_COSINE);
}
