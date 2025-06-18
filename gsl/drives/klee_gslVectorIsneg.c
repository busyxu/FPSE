#include <klee/klee.h>
#include <stdio.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_cblas.h>

int main(){
  size_t s = 6;
  double a, b, c, d, e, f;
  klee_make_symbolic(&a, sizeof(a), "a");  
  klee_make_symbolic(&b, sizeof(b), "b");
  klee_make_symbolic(&c, sizeof(c), "c");
  klee_make_symbolic(&d, sizeof(d), "d");
  klee_make_symbolic(&e, sizeof(e), "e");
  klee_make_symbolic(&f, sizeof(f), "f");
  gsl_vector * v1 = gsl_vector_calloc(s);
  gsl_vector_set(v1, 0, a);
  gsl_vector_set(v1, 1, b);
  gsl_vector_set(v1, 2, c);
  gsl_vector_set(v1, 3, d);
  gsl_vector_set(v1, 4, a);
  gsl_vector_set(v1, 5, d);
  gsl_vector_isneg(v1);
  return 0;
}
