//
// Created by liukunlin on 2022/1/17.
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

int main(){
    double a,b;
    klee_make_symbolic(&a, sizeof(a), "a");
    klee_make_symbolic(&b, sizeof(b), "b");
    double *cheb12 = (double *) malloc(sizeof(double)*30);
    double *cheb24 = (double *) malloc(sizeof(double)*30);
    double alpha = 2.6 ;
    gsl_function f = make_function(&f1,&alpha);
    gsl_integration_qcheb (&f, a, b, cheb12, cheb24);
}
