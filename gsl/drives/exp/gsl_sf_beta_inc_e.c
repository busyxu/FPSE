#include "klee/klee.h"
#include "gsl/gsl_sf_gamma.h"
int main()
{
    double a;
    klee_make_symbolic(&a,sizeof(a),"a");
    double b;
    klee_make_symbolic(&b,sizeof(b),"b");
    double c;
    klee_make_symbolic(&c,sizeof(c),"c");
    gsl_sf_result result;
    gsl_sf_beta_inc_e(a, b, c, &result);
}
