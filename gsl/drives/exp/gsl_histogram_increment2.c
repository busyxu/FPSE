//
// Created by liukunlin on 2021/12/19.
//

#include "klee/klee.h"
#include <gsl/gsl_errno.h>
#include <gsl/gsl_histogram.h>

int main(){
    size_t n = 8;
    double range[n+1];
    klee_make_symbolic(range, sizeof(range),"range");
    gsl_histogram * h = gsl_histogram_alloc(n);
    gsl_histogram_set_ranges(h,range,n+1);
    double x;
    klee_make_symbolic(&x, sizeof(double), "x");
    gsl_histogram_increment(h,x);
}