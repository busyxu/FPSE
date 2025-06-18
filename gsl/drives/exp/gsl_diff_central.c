//
// Created by liukunlin on 2021/12/19.
//

#include "klee/klee.h"
#include "gsl/gsl_math.h"
#include "gsl/gsl_diff.h"
double
df1 (double x, void *params)
{
    return exp (x);
}

int main()
{
    gsl_function F1;
    F1.function = &df1;

    double x;
    double result, abserr;

    klee_make_symbolic(&x, sizeof(double),"x");

    gsl_diff_central(&F1, x, &result, &abserr);

}