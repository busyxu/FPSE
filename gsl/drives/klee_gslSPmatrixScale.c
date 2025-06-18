#include <klee/klee.h>
#include <stdio.h>
#include <gsl/gsl_spmatrix.h>

int main(){
  double a, b, c, d;
  klee_make_symbolic(&a, sizeof(a), "a");  
  klee_make_symbolic(&b, sizeof(b), "b");
  klee_make_symbolic(&c, sizeof(c), "c");
  klee_make_symbolic(&d, sizeof(d), "d");
  double x;
  klee_make_symbolic(&x, sizeof(x), "x");
  
  gsl_spmatrix *A = gsl_spmatrix_alloc(3, 3); /* triplet format */

  /* build the sparse matrix */
  gsl_spmatrix_set(A, 0, 1, a);
  gsl_spmatrix_set(A, 0, 2, b);
  gsl_spmatrix_set(A, 1, 0, c);
  gsl_spmatrix_set(A, 1, 2, d);

  int r = gsl_spmatrix_scale(A, x);
  return 0;
}
