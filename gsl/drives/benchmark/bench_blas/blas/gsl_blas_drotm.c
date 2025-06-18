// ======== add by yx ========

#include <stdio.h>
#include <gsl/gsl_blas.h>
//#include <klee/klee.h>

int main (void)
{
  double a0=1, a1=0;
  double b0=1011, b1=1012;
  double c0=1,c1=1;

//  klee_make_symbolic(&a0, sizeof(a0), "a0");
//  klee_make_symbolic(&a1, sizeof(a1), "a1");
//  klee_make_symbolic(&b0, sizeof(b0), "b0");
//  klee_make_symbolic(&b1, sizeof(b1), "b1");
//  klee_make_symbolic(&c0, sizeof(c0), "c0");
//  klee_make_symbolic(&c1, sizeof(c1), "c1");

  double a[] = { a0, a1};
  double b[] = { b0, b1};
  double c[] = { c0, c1};

  gsl_vector_view A = gsl_vector_view_array(a, 2);
  gsl_vector_view B = gsl_vector_view_array(b, 2);

  int status = gsl_blas_drotm(&A.vector, &B.vector, c);
  printf("%d\n", status);
  //printf("%g\n", dotu);
  //gsl_vector_float_free(&A);
  //gsl_vector_float_free(&B);
  //gsl_vector_float_free(dotu);
  
  return 0;
}
