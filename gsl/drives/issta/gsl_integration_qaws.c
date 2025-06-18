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
    double a,b,c,epsabs, epsrel;
    klee_make_symbolic(&a, sizeof(a),"a");
    klee_make_symbolic(&b, sizeof(b),"b");
    double al;
    double beta;
    klee_make_symbolic(&al, sizeof(al),"al");
    klee_make_symbolic(&beta, sizeof(beta),"beta");
    int mu,nu;
    klee_make_symbolic(&mu, sizeof(mu),"mu");
    klee_make_symbolic(&nu, sizeof(nu),"nu");
    gsl_integration_qaws_table * t = gsl_integration_qaws_table_alloc(alpha,beta,mu,nu);
    klee_make_symbolic(&epsabs, sizeof(epsabs),"epsabs");
    klee_make_symbolic(&epsrel, sizeof(epsrel),"epsrel");
    size_t limit = 1000;
    gsl_integration_workspace * workspace = gsl_integration_workspace_alloc(limit);
    double result = 0.0;
    double abserr = 0.0;


    gsl_integration_qaws (&f, a,b,t,epsabs, epsrel, limit,
                          workspace, &result, &abserr);
}
