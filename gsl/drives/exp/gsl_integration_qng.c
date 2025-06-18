//
// Created by liukunlin on 2021/8/31.
//
#include "klee/klee.h"
#include "gsl/gsl_integration.h"
double
f1 (double x, void *params)
{
    return exp (x);
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
    double alpha;
    klee_make_symbolic(&alpha, sizeof(alpha),"alpha");
    gsl_function f = make_function(&f1,&alpha);
    double a,b,epsabs,epsrel;
    klee_make_symbolic(&a, sizeof(a),"a");
    klee_make_symbolic(&b, sizeof(b),"b");
    klee_make_symbolic(&epsabs, sizeof(epsabs),"epsabs");
    klee_make_symbolic(&epsrel, sizeof(epsrel),"epsrel");
    double result = 0;
    double abserr = 0;
    size_t neval=0;
    gsl_integration_qng (&f,a,b,epsabs,epsrel,&result,&abserr,&neval);


}

