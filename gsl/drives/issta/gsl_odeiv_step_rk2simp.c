//
// Created by liukunlin on 2022/1/17.
//

//
// Created by liukunlin on 2022/1/17.
//

#include "klee/klee.h"
#include "gsl/gsl_odeiv.h"
#include "gsl/gsl_errno.h"

int
rhs_vanderpol (double t, const double y[], double f[], void *params)
{
    const double mu = 10.0;

    f[0] = y[1];
    f[1] = -y[0] + mu * y[1] * (1.0 - y[0]*y[0]);

    return GSL_SUCCESS;
}

int
jac_vanderpol (double t, const double y[], double *dfdy, double dfdt[],
               void *params)
{
    const double mu = 10.0;

    dfdy[0] = 0.0;
    dfdy[1] = 1.0;
    dfdy[2] = -2.0 * mu * y[0] * y[1] - 1.0;
    dfdy[3] = mu * (1.0 - y[0] * y[0]);

    dfdt[0] = 0.0;
    dfdt[1] = 0.0;

    return GSL_SUCCESS;
}

gsl_odeiv_system rhs_func_vanderpol = {
        rhs_vanderpol,
        jac_vanderpol,
        2,
        0
};
int main(){
    gsl_odeiv_step *step  = gsl_odeiv_step_alloc(gsl_odeiv_step_rk2simp,rhs_func_vanderpol.dimension);
    double epsabs,epsrel,a_y,a_dydt;
    klee_make_symbolic(&epsabs,sizeof(epsabs),"epsabs");
    klee_make_symbolic(&epsrel,sizeof(epsrel),"epsrel");
    klee_make_symbolic(&a_y,sizeof(a_y),"a_y");
    klee_make_symbolic(&a_dydt,sizeof(a_dydt),"a_dydt");
    gsl_odeiv_control *c =
            gsl_odeiv_control_standard_new (epsabs, epsrel, a_y,a_dydt);
    gsl_odeiv_evolve *e = gsl_odeiv_evolve_alloc (rhs_func_vanderpol.dimension);
    double t,t1,h;
    double y[2];
    klee_make_symbolic(&t, sizeof(t),"t");
    klee_make_symbolic(&t1, sizeof(t1),"t1");
    klee_make_symbolic(&h, sizeof(h),"h");
    klee_make_symbolic(y, sizeof(double)*2,"y");
    gsl_odeiv_evolve_apply (e, c, step, &rhs_func_vanderpol, &t, t1, &h, y);
}