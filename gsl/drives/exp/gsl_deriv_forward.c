//
// Created by liukunlin on 2021/12/19.
//

#include "klee/klee.h"
#include "gsl/gsl_math.h"
#include "gsl/gsl_deriv.h"
double
f3 (double x, void *params)
{
    if (x != 0.0)
    {
        return sin (1 / x);
    }
    else
    {
        return 0.0;
    }
}


int main()
{
    gsl_function F1;
    F1.function = &f3;

    double x,h;
    double result, abserr;

    klee_make_symbolic(&x, sizeof(double),"x");
    klee_make_symbolic(&h, sizeof(double),"h");

    gsl_deriv_forward(&F1, x, h, &result, &abserr);

}