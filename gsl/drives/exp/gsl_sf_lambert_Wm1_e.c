#include "klee/klee.h"
#include "gsl/gsl_sf_lambert.h"
int main()
{
    double x;
    klee_make_symbolic(&x,sizeof(x),"x");
    gsl_sf_result result;
    gsl_sf_lambert_Wm1_e(x, &result);
}