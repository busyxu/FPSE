#include <klee/klee.h>
#include <stdio.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_blas.h>

int main(){
  /*
  size_t d = 2;
  double res = 0.0;
  gsl_vector * v1 = gsl_vector_calloc(d);
  gsl_vector_set(v1, 0, 2.0);
  gsl_vector_set(v1, 1, 4.0);
  res = gsl_blas_dnrm2(v1);
  printf("%f", res);
  */
  size_t d = 2;
  double res = 0.0;
  double a, b;
  klee_make_symbolic(&a, sizeof(a), "a");
  klee_make_symbolic(&b, sizeof(b), "b");
  gsl_vector * v1 = gsl_vector_calloc(d);
  gsl_vector_set(v1, 0, a);
  gsl_vector_set(v1, 1, b);
  res = gsl_blas_dnrm2(v1); 
  return 0;
}
