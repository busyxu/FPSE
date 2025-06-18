//
// Created by liukunlin on 2021/8/19.
//
#include "klee/klee.h"
#include "gsl/gsl_fft_real.h"
int main()
{
    size_t n = 4;
    double data[n];
    size_t stride;
    klee_make_symbolic(&data, sizeof(data),"data");
    klee_make_symbolic(&stride, sizeof(stride),"stride");
    gsl_fft_real_radix2_transform (data, stride, n);
}