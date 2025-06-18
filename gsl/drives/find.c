//
// Created by liukunlin on 2021/8/19.
//
#include "klee/klee.h"
#include <gsl/gsl_histogram.h>
int main()
{
    size_t n = 3;
    double range[n+1];
//    double bin[n];
    klee_make_symbolic(range, sizeof(range),"range");
//    for(int i=1;i<n+1;i++)
//    {
//        klee_assume(range[i-1]<range[i]);
//    }
    double x;
    klee_make_symbolic(&x, sizeof(x),"x");
    size_t i;
    gsl_histogram *h = gsl_histogram_alloc(n);
    gsl_histogram_set_ranges(h,range,n+1);
//    gsl_histogram h;
//    h.range = range;
//    h.n = n;
//    h.bin = bin;
    gsl_histogram_find(h, x, &i);
}