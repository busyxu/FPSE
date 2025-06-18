// ======== add by yx ========
#include <stdio.h>
#include <gsl/gsl_blas.h>
//#include <klee/klee.h>

int main (void)
{
  float a0=1, a1=0;
  float b0=1011, b1=1012;
  float c0=1,c1=0;
  float s0=1,s1=1;
//  klee_make_symbolic(&a0, sizeof(a0), "a0");
//  klee_make_symbolic(&a1, sizeof(a1), "a1");
//  klee_make_symbolic(&b0, sizeof(b0), "b0");
//  klee_make_symbolic(&b1, sizeof(b1), "b1");
//  klee_make_symbolic(&c0, sizeof(c0), "c0");
//  klee_make_symbolic(&c1, sizeof(c1), "c1");
//  klee_make_symbolic(&s0, sizeof(s0), "s0");
//  klee_make_symbolic(&s1, sizeof(s1), "s1");

  float a[] = { a0, a1};
  float b[] = { b0, b1};
  float c[] = { c0, c1};
  float s[] = { s0, s1};

  int status = gsl_blas_srotg(a, b, c, s);
  printf("%d\n", status);
  //printf("%g\n", dotu);
  //gsl_vector_float_free(&A);
  //gsl_vector_float_free(&B);
  //gsl_vector_float_free(dotu);
  
  return 0;
}
