#include <stdio.h>
#include <gsl/gsl_blas.h>
//#include <klee/klee.h>

int main (void)
{

  float a0=1, a1=0;
  float b0=1011, b1=1012;
  float c0=0.21, c1=0.22, c2=0.23, c3=0.00;
  float alpha=1, beta=1;
  size_t Uplo = CblasUpper;
//  klee_make_symbolic(&alpha, sizeof(alpha), "alpha");
//  klee_make_symbolic(&beta, sizeof(beta), "beta");
//  klee_make_symbolic(&Uplo, sizeof(Uplo), "Uplo");
//  klee_make_symbolic(&a0, sizeof(a0), "a0");
//  klee_make_symbolic(&a1, sizeof(a1), "a1");
//  klee_make_symbolic(&b0, sizeof(b0), "b0");
//  klee_make_symbolic(&b1, sizeof(b1), "b1");
//  klee_make_symbolic(&c0, sizeof(c0), "c0");
//  klee_make_symbolic(&c1, sizeof(c1), "c1");
//  klee_make_symbolic(&c2, sizeof(c2), "c2");
//  klee_make_symbolic(&c3, sizeof(c3), "c3");

  float a[] = { a0, a1};
  float b[] = { b0, b1};
  float c[] = { c0, c1, c2, c3};

  gsl_vector_float_view A = gsl_vector_float_view_array(a, 2);
  gsl_vector_float_view B = gsl_vector_float_view_array(b, 2);
  gsl_matrix_float_view C = gsl_matrix_float_view_array(c, 2, 2);

  int status = gsl_blas_ssymv(Uplo, alpha, &C.matrix, &A.vector, beta, &B.vector);
  printf("%d\n", status);
  //printf ("[ %g, %g\n", c[0], c[1]);
  //printf ("  %g, %g ]\n", c[2], c[3]);

  return 0;
}
