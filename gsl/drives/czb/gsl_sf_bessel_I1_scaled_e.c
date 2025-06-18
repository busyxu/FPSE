#include "klee/klee.h"
#include "gsl/gsl_sf_bessel.h"
int main()
{
    double x;
    klee_make_symbolic(&x,sizeof(x),"x");
    gsl_sf_result result;
    gsl_sf_bessel_I1_scaled_e(x, &result);
}
