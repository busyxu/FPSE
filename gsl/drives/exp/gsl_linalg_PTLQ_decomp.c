//
// Created by liukunlin on 2021/8/30.
//

#include "klee/klee.h"
#include "gsl/gsl_linalg.h"
void symbolizeMatrix(gsl_matrix *m, size_t n1, size_t n2) {
    char name[3] = {'m', 'm', 0};
    for (int i = 0; i < n1 ; i++) {
        for (int j = 0; j < n2; j++) {
            double *p = malloc(sizeof(double));
            name[1] = '0' + i*n1 + j;
            klee_make_symbolic(p, sizeof(double), name);
            gsl_matrix_set(m, i, j, *p);
        }
    }
}
int main()
{
    size_t M=3;
    size_t N=3;
    gsl_matrix * A = gsl_matrix_alloc(M,N);
    symbolizeMatrix(A,M,N);
    gsl_vector *tau = gsl_vector_alloc(M);
    gsl_permutation * p = gsl_permutation_alloc(M);
    gsl_vector *norm = gsl_vector_alloc(M);
    int signum;
    gsl_linalg_PTLQ_decomp(A,tau,p,&signum,norm);
}