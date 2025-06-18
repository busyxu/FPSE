#include <klee/klee.h>
#include <stdio.h>
#include <gsl/gsl_blas.h>

int main(){
    //klee_make_symbolic(data, 100 * sizeof(double), "data");
  /**
  double a[6];
  double b[6];
  double c[4] = { 0.00, 0.00,
                 0.00, 0.00 };

  klee_make_symbolic(a, 6 * sizeof(double), "a");
  klee_make_symbolic(b, 6 * sizeof(double), "b");
  gsl_matrix_view A = gsl_matrix_view_array(a, 2, 3);
  gsl_matrix_view B = gsl_matrix_view_array(b, 3, 2);
  gsl_matrix_view C = gsl_matrix_view_array(c, 2, 2);

  gsl_blas_dgemm (CblasNoTrans, CblasNoTrans,
                  1.0, &A.matrix, &B.matrix,
                  0.0, &C.matrix);
  return 0;
  **/

  double a[3] = {1.0, 2.0, 3.0};
  double b[3] = {1.0, 2.0, 3.0};
  double res;
  gsl_blas_dsdot(a, b, &res);
  return 0;  
}
