// ======== add by yx ========

#include <stdio.h>
#include <gsl/gsl_blas.h>
//#include <klee/klee.h>

int main (void)
{
  float a0=1, a1=0;
  float b0=1011, b1=1012, b2=1021, b3=0.11;
  size_t Uplo = CblasUpper;
  size_t TransA = CblasNoTrans;
  size_t Diag = CblasNonUnit;

//  klee_make_symbolic(&Uplo, sizeof(Uplo), "Uplo");
//  klee_make_symbolic(&TransA, sizeof(TransA), "TransA");
//  klee_make_symbolic(&Diag, sizeof(Diag), "Diag");
//  klee_make_symbolic(&a0, sizeof(a0), "a0");
//  klee_make_symbolic(&a1, sizeof(a1), "a1");
//  klee_make_symbolic(&b0, sizeof(b0), "b0");
//  klee_make_symbolic(&b1, sizeof(b1), "b1");
//  klee_make_symbolic(&b2, sizeof(b2), "b2");
//  klee_make_symbolic(&b3, sizeof(b3), "b3");

  float a[] = { a0, a1};
  float b[] = { b0, b1, b2, b3};

  gsl_vector_float_view X = gsl_vector_float_view_array(a, 2);
  gsl_matrix_float_view A = gsl_matrix_float_view_array(b, 2, 2);

  int status = gsl_blas_strsv(Uplo, TransA, Diag, &A.matrix, &X.vector);
  printf ("%d\n", status);

  return 0;
}
