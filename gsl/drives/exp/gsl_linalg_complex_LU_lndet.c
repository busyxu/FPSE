//
// Created by liukunlin on 2021/8/30.
//
#include "klee/klee.h"
#include "gsl/gsl_linalg.h"

void symbolizeMatrix(gsl_matrix_complex *m, size_t n1, size_t n2) {
    char namex[3] = {'x', 'x', 0};
    char namey[3] = {'y', 'y', 0};
    for (int i = 0; i < n1 ; i++) {
        for (int j = 0; j < n2; j++) {
            double *x = malloc(sizeof(double));
            double *y = malloc(sizeof(double));
            namex[1] = '0' + i*n1 + j;
            namey[1] = '0' + i*n1 + j;
            klee_make_symbolic(x, sizeof(x),namex);
            klee_make_symbolic(y, sizeof(y),namey);
            gsl_complex *i1 = malloc(sizeof(gsl_complex));
            i1->dat[0] = *x;
            i1->dat[1] = *y;
            gsl_matrix_complex_set(m, i, j, *i1);
        }
    }
}

int main()
{
    size_t M=3;
    size_t N=3;
    double x1,y1;
    gsl_matrix_complex *LU = gsl_matrix_complex_alloc(M,N);

    symbolizeMatrix(LU,M,N);
    gsl_linalg_complex_LU_lndet (LU);
}