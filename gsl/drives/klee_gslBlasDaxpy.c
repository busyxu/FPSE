#include <klee/klee.h>
#include <stdio.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_blas.h>

int main(){
  size_t s = 2;
  double a, b, c, d, alpha;
  klee_make_symbolic(&a, sizeof(a), "a");  
  klee_make_symbolic(&b, sizeof(b), "b");
  klee_make_symbolic(&c, sizeof(c), "c");
  klee_make_symbolic(&d, sizeof(d), "d");
  klee_make_symbolic(&alpha, sizeof(alpha), "alpha");
  gsl_vector * v1 = gsl_vector_calloc(s);
  gsl_vector * v2 = gsl_vector_calloc(s);
  gsl_vector_set(v1, 0, a);
  gsl_vector_set(v1, 1, b);
  gsl_vector_set(v2, 0, c);
  gsl_vector_set(v2, 1, d);
  gsl_blas_daxpy(alpha, v1, v2); 
  return 0;
}
