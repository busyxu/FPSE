//
// Created by liukunlin on 2021/12/19.
//

#include "klee/klee.h"
#include <gsl/gsl_errno.h>
#include <gsl/gsl_histogram.h>

int main(){
    size_t n = 8;
    double range1[n+1];
    double range2[n+1];
    klee_make_symbolic(&range1, sizeof(range1), "range1");
    klee_make_symbolic(&range2, sizeof(range2), "range2");
    gsl_histogram * h1 = gsl_histogram_alloc(n);
    gsl_histogram_set_ranges(h1,range1,n+1);
    gsl_histogram * h2 = gsl_histogram_alloc(n);
    gsl_histogram_set_ranges(h2,range2,n+1);
    gsl_histogram_equal_bins_p(h1,h2);
}