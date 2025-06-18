
//
// Created by liukunlin on 2021/8/19.
//
#include "klee/klee.h"
#include "gsl/gsl_sum.h"
int main()
{
    size_t array_size = 5;
    double array[5];
    klee_make_symbolic(array, sizeof(double)*5,"array");
    size_t min_terms,max_terms;
    klee_make_symbolic(&min_terms, sizeof(min_terms),"min_terms");
    klee_make_symbolic(&max_terms, sizeof(max_terms),"max_terms");
//    klee_assume(min_terms<5 && min_terms>0);
//    klee_assume(max_terms<5 && max_terms>0);
    double sum_accel,abserr;
    gsl_sum_levin_u_workspace *w = gsl_sum_levin_u_alloc(array_size);

    gsl_sum_levin_u_minmax (array, array_size,
                            min_terms, max_terms,
                            w,
                            &sum_accel, &abserr);
}
