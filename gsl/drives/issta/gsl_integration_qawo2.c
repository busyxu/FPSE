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
    double alpha = 2.6 ;
    gsl_function f = make_function(&f1,&alpha);
    double a,epsabs,epsrel;
    klee_make_symbolic(&a, sizeof(a),"a");
    klee_make_symbolic(&epsabs, sizeof(epsabs),"epsabs");
    klee_make_symbolic(&epsrel, sizeof(epsrel),"epsrel");
    size_t limit = 1000;
    double result, abserr;
    gsl_integration_workspace * workspace = gsl_integration_workspace_alloc(limit);
    double omega,L;
    klee_make_symbolic(&omega, sizeof(omega),"omega");
    klee_make_symbolic(&L, sizeof(L),"L");
    gsl_integration_qawo_table * wf  = gsl_integration_qawo_table_alloc (omega, L,GSL_INTEG_SINE,limit);
    gsl_integration_qawo (&f,a,epsabs, epsrel,limit, workspace, wf,&result, &abserr);
}