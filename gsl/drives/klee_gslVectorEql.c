#include <klee/klee.h>
#include <stdio.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_cblas.h>

int main(){
  size_t s = 3;
  double a, b, c, d, e, f;
  klee_make_symbolic(&a, sizeof(a), "a");  
  klee_make_symbolic(&b, sizeof(b), "b");
  klee_make_symbolic(&c, sizeof(c), "c");
  klee_make_symbolic(&d, sizeof(d), "d");
  klee_make_symbolic(&e, sizeof(e), "e");
  klee_make_symbolic(&f, sizeof(f), "f");
  gsl_vector * v1 = gsl_vector_calloc(s);
  gsl_vector * v2 = gsl_vector_calloc(s);
  gsl_vector_set(v1, 0, a);
  gsl_vector_set(v1, 1, b);
  gsl_vector_set(v1, 2, c);
  gsl_vector_set(v2, 0, d);
  gsl_vector_set(v2, 1, a);
  gsl_vector_set(v2, 2, d);
  gsl_vector_equal(v1, v2);
  return 0;
}
