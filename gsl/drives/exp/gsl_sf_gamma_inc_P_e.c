#include "klee/klee.h"
#include "gsl/gsl_sf_gamma.h"
int main()
{
    double a;
    klee_make_symbolic(&a,sizeof(a),"a");
    double x;
    klee_make_symbolic(&x,sizeof(x),"x");

    gsl_sf_result result;
    gsl_sf_gamma_inc_P_e(a, x, &result);
}
