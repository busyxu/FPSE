//
// Created by liukunlin on 2021/8/31.
//
#include "klee/klee.h"
#include "gsl/gsl_integration.h"


void symbolizea(double *v, int len) {
    char name[3] = {'a', 'a', 0};
    for (int i = 0; i < len; i++) {
        double *p = malloc(sizeof(double));
        name[1] = '0' + i;
        klee_make_symbolic(p, sizeof(double), name);
        v[i] = *p;
    }
}

void symbolizeb(double *v, int len) {
    char name[3] = {'b', 'b', 0};
    for (int i = 0; i < len; i++) {
        double *p = malloc(sizeof(double));
        name[1] = '0' + i;
        klee_make_symbolic(p, sizeof(double), name);
        v[i] = *p;
    }
}
void symbolizec(double *v, int len) {
    char name[3] = {'c', 'c', 0};
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
    double alpha = 2.6 ;
    gsl_function f = make_function(&f1,&alpha);
    int n = 9;
    double xgk[n];
    double wg[n];
    double wgk[n];
    symbolizea(xgk,n);
    symbolizeb(wg,n);
    symbolizec(wgk,n);
    double a,b;
    klee_make_symbolic(&a, sizeof(a),"a");
    double fv1[n];
    double fv2[n];
    double result = 0.0;
    double abserr = 0.0;
    double resabs = 0.0;
    double resasc =0.0;


    gsl_integration_qk (n,xgk,wg,wgk,fv1,fv2,&f, a,b, &result,&abserr,&resabs,&resasc);
}
