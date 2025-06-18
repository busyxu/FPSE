
//
// Created by liukunlin on 2021/8/19.
//
#include "klee/klee.h"
#include "gsl/gsl_sort.h"
int main()
{
    double data[10];
    klee_make_symbolic(data, 10 * sizeof(double), "data");
    gsl_sort(data, 1, 10);
    return 0;
}
