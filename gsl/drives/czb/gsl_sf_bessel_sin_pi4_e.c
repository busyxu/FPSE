#include "klee/klee.h"
#include "gsl/gsl_sf_bessel.h"
int main()
{
    double y;
    klee_make_symbolic(&y,sizeof(y),"y");
    double eps;
    klee_make_symbolic(&eps,sizeof(eps),"eps");
    gsl_sf_result result;
    gsl_sf_bessel_sin_pi4_e(y, eps, &result);
}
