//
// Created by liukunlin on 2021/8/19.
//
#include "klee/klee.h"
#include "gsl/gsl_complex_math.h"
#include "gsl/gsl_complex.h"

int main()
{
    double x;
    double y;
    klee_make_symbolic(&x, sizeof(x),"x");
    klee_make_symbolic(&y, sizeof(y),"y");
    gsl_complex i;
    i.dat[0] = x;
    i.dat[1] = y;
    gsl_complex_sin(i);
}
