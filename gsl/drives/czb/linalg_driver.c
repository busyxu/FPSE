#include "klee/klee.h"
#include "gsl/gsl_matrix_double.h"
#include "gsl/gsl_linalg.h"

/// for matrix Factorise

/// folder: lnalg

void symbolizeMatrix(gsl_matrix *m, int n1, int n2) {
    char name[3] = {'a', 'a', 0};
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
    int M = 3, N = 3;

    gsl_matrix * v  = gsl_matrix_alloc(M,N);
    gsl_matrix * q  = gsl_matrix_alloc(N,N);
    gsl_vector * d  = gsl_vector_alloc(N);
    gsl_vector * w  = gsl_vector_alloc(N);

    symbolizeMatrix(v, M, N);

    gsl_linalg_SV_decomp(v, q, d, w);
}
