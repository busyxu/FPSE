//
// Created by liukunlin on 2021/8/19.
//
#include "klee/klee.h"
#include "gsl/gsl_eigen.h"
#include "gsl/gsl_complex_math.h"
int main()
{
    size_t n=3;
//    klee_make_symbolic(&n, sizeof(n), "n");
    gsl_vector_complex * alpha = gsl_vector_complex_alloc(n);
    gsl_complex a1;
    gsl_complex a2;
    gsl_complex a3;
    klee_make_symbolic(&a1, sizeof(a1),"alpha1");
    klee_make_symbolic(&a2, sizeof(a2),"alpha2");
    klee_make_symbolic(&a3, sizeof(a3),"alpha3");
    gsl_vector_complex_set(alpha,0,a1);
    gsl_vector_complex_set(alpha,1,a2);
    gsl_vector_complex_set(alpha,2,a3);
    gsl_vector * beta = gsl_vector_alloc(n);
    double b1;
    double b2;
    double b3;
    klee_make_symbolic(&b1, sizeof(b1),"beta1");
    klee_make_symbolic(&b2, sizeof(b2),"beta2");
    klee_make_symbolic(&b3, sizeof(b3),"beta3");
    gsl_vector_set(beta,0,b1);
    gsl_vector_set(beta,1,b2);
    gsl_vector_set(beta,2,b3);
    gsl_matrix_complex * evec = gsl_matrix_complex_alloc(n,n);
    gsl_eigen_genv_sort (alpha, beta,
                         evec, GSL_EIGEN_SORT_ABS_ASC);
}
