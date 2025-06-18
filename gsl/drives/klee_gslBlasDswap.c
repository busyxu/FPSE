#include <klee/klee.h>
#include <stdio.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_blas.h>

int main(){
  /*
  size_t d = 2;
  int res = 0;
  gsl_vector * v1 = gsl_vector_calloc(d);
  gsl_vector * v2 = gsl_vector_calloc(d);
  gsl_vector_set(v1, 0, 2.0);
  gsl_vector_set(v1, 1, 2.0);
  gsl_vector_set(v2, 0, 1.0);
  gsl_vector_set(v2, 1, 1.0);
  gsl_blas_dswap(v1, v2);
  printf("%d", res);   
  */
  size_t s = 2;
  double a, b, c, d;
  klee_make_symbolic(&a, sizeof(a), "a");  
  klee_make_symbolic(&b, sizeof(b), "b");
  klee_make_symbolic(&c, sizeof(c), "c");
  klee_make_symbolic(&d, sizeof(d), "d");
  gsl_vector * v1 = gsl_vector_calloc(s);
  gsl_vector * v2 = gsl_vector_calloc(s);
  gsl_vector_set(v1, 0, a);
  gsl_vector_set(v1, 1, b);
  gsl_vector_set(v2, 0, c);
  gsl_vector_set(v2, 1, d);
  gsl_blas_dswap(v1, v2); 
  return 0;
}
