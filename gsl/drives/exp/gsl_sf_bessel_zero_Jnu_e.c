#include "klee/klee.h"
#include "gsl/gsl_sf_bessel.h"
int main()
{
    double nu;
    klee_make_symbolic(&nu,sizeof(nu),"nu");
    unsigned int s;
    klee_make_symbolic(&s, sizeof(s), "s");
    gsl_sf_result result;
    gsl_sf_bessel_zero_Jnu_e(nu, s, &result);
}
