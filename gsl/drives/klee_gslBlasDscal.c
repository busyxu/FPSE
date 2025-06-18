#include <klee/klee.h>
#include <stdio.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_blas.h>

int main(){
  size_t s = 2;
  double a, b, x1;
  klee_make_symbolic(&a, sizeof(a), "a");  
  klee_make_symbolic(&b, sizeof(b), "b");
  klee_make_symbolic(&x1, sizeof(x1), "x1");
  gsl_vector * v1 = gsl_vector_calloc(s);
  gsl_vector * v2 = gsl_vector_calloc(s);
  gsl_vector_set(v1, 0, a);
  gsl_vector_set(v1, 1, b);
  gsl_blas_dscal(x1, v1); 
  return 0;
}
