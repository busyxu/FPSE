#include "klee/klee.h"
#include "gsl/gsl_sf_debye.h"
int main()
{
    double x;
    klee_make_symbolic(&x,sizeof(x),"x");
    gsl_sf_result result;
    gsl_sf_debye_1_e(x, &result);
}
