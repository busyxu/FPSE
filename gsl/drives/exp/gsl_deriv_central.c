#include "klee/klee.h"
#include "gsl/gsl_math.h"
#include "gsl/gsl_deriv.h"

/// folder: drives

/// see the test.c in deriv

double
f1 (double x, void *params)
{
    return exp (x);
}


int main()
{
    gsl_function F1;
    F1.function = &f1;

    double x,h;
    double result, abserr;

    klee_make_symbolic(&x, sizeof(double),"x");
    klee_make_symbolic(&h, sizeof(double),"h");

    gsl_deriv_central(&F1, x, h, &result, &abserr);

}
