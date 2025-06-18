//
// Created by liukunlin on 2021/8/19.
//
#include "klee/klee.h"
#include <math.h>
#include "gsl/gsl_integration.h"
#include "gsl/gsl_diff.h"


/// folder: integration
/// try different f functions, see the test.c program

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
    double result = 0, abserr = 0, resabs = 0, resasc = 0 ;

    double alpha = 2.6 ;
    gsl_function f = make_function(&f1, &alpha);


    double a,b;
    klee_make_symbolic(&a, sizeof(a),"a");
    klee_make_symbolic(&b, sizeof(b),"b");

    gsl_integration_qk21 (&f, a, b, &result, &abserr, &resabs, &resasc) ;

}
