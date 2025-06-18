//
// Created by liukunlin on 2021/8/30.
//
#include "klee/klee.h"
#include "gsl/gsl_interp.h"


void symbolizea(double *s, int len) {
    char name[3] = {'a', 'a', 0};
    for (int i = 0; i < len; i++) {
        double *p = malloc(sizeof(double));
        name[1] = '0' + i;
        klee_make_symbolic(p, sizeof(double), name);
        s[i] = *p;
    }
}
void symbolizeb(double *s, int len) {
    char name[3] = {'b', 'b', 0};
    for (int i = 0; i < len; i++) {
        double *p = malloc(sizeof(double));
        name[1] = '0' + i;
        klee_make_symbolic(p, sizeof(double), name);
        s[i] = *p;
    }
}

int main()
{
    size_t n = 5;
    gsl_interp *interp = gsl_interp_alloc(gsl_interp_cspline,n);
    double xa[n];
    symbolizea(xa,n);
    double ya[n];
    symbolizeb(ya,n);
    double a,b;
    klee_make_symbolic(&a, sizeof(a),"a");
    klee_make_symbolic(&b, sizeof(b),"b");
    gsl_interp_init(interp,xa,ya,n);
    gsl_interp_accel acc;
    klee_make_symbolic(&acc, sizeof(acc), sizeof(acc));
    gsl_interp_eval_integ(interp,xa,ya,a,b,&acc);

}
