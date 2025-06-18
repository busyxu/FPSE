/*=====add by yx =====*/

#include <stdio.h>
#include <gsl/gsl_blas.h>
#include <klee/klee.h>

int main (void)
{

  double a[] = { 0.11, 0.12, 0.13, 0.21, 0.22, 0.23 };
  double alpha = 1.0;

  klee_make_symbolic(&a[0], sizeof(a[0]), "a0");
  klee_make_symbolic(&a[1], sizeof(a[1]), "a1");
  klee_make_symbolic(&a[2], sizeof(a[2]), "a2");
  klee_make_symbolic(&a[3], sizeof(a[3]), "a3");
  klee_make_symbolic(&a[4], sizeof(a[4]), "a4");
  klee_make_symbolic(&a[5], sizeof(a[5]), "a5");
  klee_make_symbolic(&alpha, sizeof(alpha), "alpha");

  gsl_vector_view A = gsl_vector_view_array(a, 6);

  gsl_blas_dscal(alpha,&A.vector);
  //printf("%g\n", alpha);
  //gsl_vector_float_free(&A);
  //gsl_vector_float_free(&B);
  //gsl_vector_float_free(dotu);
  
  return 0;
}
