//
// Created by liukunlin on 2021/8/31.
//
#include "klee/klee.h"
#include "gsl/gsl_integration.h"


void symbolizeVector(double *v, size_t len) {
    char name[3] = {'a', 'a', 0};
    for (int i = 0; i < len; i++) {
        double *p = malloc(sizeof(double));
        name[1] = '0' + i;
        klee_make_symbolic(p, sizeof(double), name);
        v[i] = *p;
    }
}
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
    double a,b,epsabs, epsrel;
    klee_make_symbolic(&a, sizeof(a),"a");
    klee_make_symbolic(&b, sizeof(b),"b");
    klee_make_symbolic(&epsabs, sizeof(epsabs),"epsabs");
    klee_make_symbolic(&epsrel, sizeof(epsrel),"epsrel");
    size_t limit = 1000;
    gsl_integration_workspace * workspace = gsl_integration_workspace_alloc(limit);
    double result = 0.0;
    double abserr = 0.0;


    gsl_integration_qags (&f, a,b,epsabs, epsrel, limit,
                          workspace, &result, &abserr);
}
