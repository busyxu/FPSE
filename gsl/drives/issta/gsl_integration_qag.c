//
// Created by liukunlin on 2021/8/31.
//
#include "klee/klee.h"
#include "gsl/gsl_integration.h"
double
f1 (double x, void *params)
{
    return sqrt (x);
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
    double alpha = 2.6 ;
    gsl_function f = make_function(&f1,&alpha);
    double a,b;
    klee_make_symbolic(&a, sizeof(a),"a");
    klee_make_symbolic(&b, sizeof(b),"b");
    double epsabs, epsrel;
    klee_make_symbolic(&epsabs, sizeof(epsabs),"epsabs");
    klee_make_symbolic(&epsrel, sizeof(epsrel),"epsrel");

    size_t limit = 1000;
    double result = 0.1;
    double abserr = 0.0;
    int key;
    klee_make_symbolic(&key, sizeof(key),"key");
    gsl_integration_workspace * workspace = gsl_integration_workspace_alloc(limit);

    gsl_integration_qag (&f,
    a, b,
    epsabs, epsrel, limit,
    key,
    workspace,
    &result, &abserr);
}

