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
    double a,b;
    klee_make_symbolic(&a, sizeof(a),"a");
    klee_make_symbolic(&b, sizeof(b),"b");
    gsl_integration_glfixed_table * t = gsl_integration_glfixed_table_alloc(1000);
    gsl_integration_glfixed (&f,a,b,t);


}

