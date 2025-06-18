#include "klee/klee.h"
#include "gsl/gsl_sf_exp.h"
int main()
{
    double x;
    klee_make_symbolic(&x,sizeof(x),"x");
    double y;
    klee_make_symbolic(&y,sizeof(y),"y");

    gsl_sf_result result;
    gsl_sf_exp_mult_e(x, y, &result);
}
