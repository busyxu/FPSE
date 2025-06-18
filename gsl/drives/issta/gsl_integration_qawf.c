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
    double a,epsabs, epsrel;
    klee_make_symbolic(&a, sizeof(a),"a");
    klee_make_symbolic(&epsabs, sizeof(epsabs),"epsabs");
    klee_make_symbolic(&epsrel, sizeof(epsrel),"epsrel");
    size_t limit = 1000;
    gsl_integration_workspace * w = gsl_integration_workspace_alloc(limit);
    gsl_integration_workspace * wc = gsl_integration_workspace_alloc(limit);
    ///symbolic?
    double omega;
    klee_make_symbolic(&omega, sizeof(omega),"omega");
    gsl_integration_qawo_table * wf = gsl_integration_qawo_table_alloc(omega, 1.0,
                                                                       GSL_INTEG_COSINE, 1000);

    double result = 0.0;
    double abserr = 0.0;


    gsl_integration_qawf (&f, a,epsabs, limit,
                          w,wc, wf,&result, &abserr);
}
