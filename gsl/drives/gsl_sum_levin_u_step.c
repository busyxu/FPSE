//
// Created by liukunlin on 2021/8/28.
//
#include "klee/klee.h"
#include "gsl/gsl_sum.h"
int main()
{
    double term;
    size_t n=5;
    size_t nmax;
    gsl_sum_levin_u_workspace * w = gsl_sum_levin_u_alloc(n);
    double sum_accel;
    klee_make_symbolic(&term,sizeof(term),"term");
    klee_make_symbolic(&nmax,sizeof(nmax),"namx");
    klee_make_symbolic(&sum_accel,sizeof(sum_accel),"sum_accel");
    gsl_sum_levin_u_step (term, n, nmax,w, &sum_accel);
}
