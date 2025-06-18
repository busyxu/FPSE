#include "klee/klee.h"
#include "gsl/gsl_blas.h"


/// folder: blas and cblas

/// there are so many functions to be tested inside cblas, inspect the h files

double
f1 (double x, void *params)
{
    return exp (x);
}

double
df1 (double x, void *params)
{
    return exp (x);
}


int main()
{
    double d1, d2, b1, b2, P[5]= { -999.0, -999.1, -999.2, -999.3, -999.4 };

    klee_make_symbolic(&d1, sizeof(double),"d1");
    klee_make_symbolic(&d2, sizeof(double),"d2");
    klee_make_symbolic(&b1, sizeof(double),"b1");
    klee_make_symbolic(&b2, sizeof(double),"b2");

    cblas_drotmg (&d1, &d2, &b1, b2, P);
}
