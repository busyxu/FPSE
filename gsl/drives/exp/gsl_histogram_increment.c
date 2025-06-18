//
// Created by liukunlin on 2021/12/19.
//

#include "klee/klee.h"
#include <gsl/gsl_errno.h>
#include <gsl/gsl_histogram.h>

int main(){
    double xmin;
    double xmax;
    size_t n = 8;
    klee_make_symbolic(&xmin, sizeof(double), "xmin");
    klee_make_symbolic(&xmax, sizeof(double), "xmax");
    gsl_histogram * h = gsl_histogram_calloc_uniform(n,xmin,xmax);
    double x;
    klee_make_symbolic(&x, sizeof(double), "x");
    gsl_histogram_increment(h,x);
}
