// ======== add by yx ========
#include <stdio.h>
#include <gsl/gsl_blas.h>
//#include <klee/klee.h>

int main (void)
{
  double a0=1, a1=0, a2=1, a3=0, a4=1, a5=0, a6=1, a7=0;
  double b0=1011, b1=1012, b2=1021, b3=0.11, b4=1011, b5=1012, b6=1021, b7=0.11;
  double alpha=1, beta=1;
  size_t Uplo = CblasUpper;
  size_t Trans = CblasNoTrans;

//  klee_make_symbolic(&alpha, sizeof(alpha), "alpha");
//  klee_make_symbolic(&beta, sizeof(beta), "beta");
//  klee_make_symbolic(&Uplo, sizeof(Uplo), "Uplo");
//  klee_make_symbolic(&Trans, sizeof(Trans), "Trans");
//  klee_make_symbolic(&a0, sizeof(a0), "a0");
//  klee_make_symbolic(&a1, sizeof(a1), "a1");
//  klee_make_symbolic(&a2, sizeof(a2), "a2");
//  klee_make_symbolic(&a3, sizeof(a3), "a3");
//  klee_make_symbolic(&a4, sizeof(a4), "a4");
//  klee_make_symbolic(&a5, sizeof(a5), "a5");
//  klee_make_symbolic(&a6, sizeof(a6), "a6");
//  klee_make_symbolic(&a7, sizeof(a7), "a7");
//  klee_make_symbolic(&b0, sizeof(b0), "b0");
//  klee_make_symbolic(&b1, sizeof(b1), "b1");
//  klee_make_symbolic(&b2, sizeof(b2), "b2");
//  klee_make_symbolic(&b3, sizeof(b3), "b3");
//  klee_make_symbolic(&b4, sizeof(b4), "b4");
//  klee_make_symbolic(&b5, sizeof(b5), "b5");
//  klee_make_symbolic(&b6, sizeof(b6), "b6");
//  klee_make_symbolic(&b7, sizeof(b7), "b7");

  double a[] = { a0, a1, a2, a3, a4, a5, a6, a7};
  double b[] = { b0, b1, b2, b3, b4, b5, b6, b7};

  gsl_matrix_complex_view A = gsl_matrix_complex_view_array(a, 2, 2);
  gsl_matrix_complex_view B = gsl_matrix_complex_view_array(b, 2, 2);

  int status = gsl_blas_zherk(Uplo, Trans, alpha, &A.matrix, beta, &B.matrix);
  printf("%d\n", status);

  return 0;
}
