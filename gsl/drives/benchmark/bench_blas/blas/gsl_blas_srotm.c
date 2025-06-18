// ======== add by yx ========

#include <stdio.h>
#include <gsl/gsl_blas.h>
//#include <klee/klee.h>

int main (void)
{
  float a0=1, a1=0;
  float b0=1011, b1=1012;
  float c0=1,c1=0;
//  klee_make_symbolic(&a0, sizeof(a0), "a0");
//  klee_make_symbolic(&a1, sizeof(a1), "a1");
//  klee_make_symbolic(&b0, sizeof(b0), "b0");
//  klee_make_symbolic(&b1, sizeof(b1), "b1");
//  klee_make_symbolic(&c0, sizeof(c0), "c0");
//  klee_make_symbolic(&c1, sizeof(c1), "c1");

  float a[] = { a0, a1};
  float b[] = { b0, b1};
  float c[] = { c0, c1};

  gsl_vector_float_view A = gsl_vector_float_view_array(a, 6);
  gsl_vector_float_view B = gsl_vector_float_view_array(b, 6);

  int status = gsl_blas_srotm(&A.vector, &B.vector, c);
  printf("%d\n", status);
  //printf("%g\n", dotu);
  //gsl_vector_float_free(&A);
  //gsl_vector_float_free(&B);
  //gsl_vector_float_free(dotu);
  
  return 0;
}
