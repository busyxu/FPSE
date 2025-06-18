//
// Created by liukunlin on 2021/8/31.
//
#include "klee/klee.h"
#include "gsl/gsl_integration.h"

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

gsl_function make_function (double (* f) (double, void *), double * p)
{
    gsl_function f_new;

    f_new.function = f ;
    f_new.params = p ;

    return f_new;
}
int main()
{

    double a ,b;
    double epsabs, epsrel;
    klee_make_symbolic(&a, sizeof(a),"a");
    klee_make_symbolic(&b, sizeof(b),"b");
    klee_make_symbolic(&epsabs, sizeof(epsabs),"epsabs");
    klee_make_symbolic(&epsrel, sizeof(epsrel),"epsrel");
    double alpha = 2.6 ;
    gsl_function f = make_function(&f3,&alpha);
    size_t n =200;
    double result = 0.0;
    double abserr = 0.0;
    size_t nevals = 0;
    gsl_integration_cquad_workspace * ws = gsl_integration_cquad_workspace_alloc(n);
    gsl_integration_cquad(&f,a,b,epsabs,epsrel,ws,&result,&abserr,&nevals);
}
