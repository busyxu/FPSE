#include <stdio.h>
#include <gsl/gsl_blas.h>
#include <klee/klee.h>

int main (void)
{
  
  //double c1,c2,c3  

  double a[] = { 0.11, 0.12, 0.13,
                 0.21, 0.22, 0.23 };

  klee_make_symbolic(&a[0], sizeof(a[0]), "a0");
  klee_make_symbolic(&a[1], sizeof(a[1]), "a1");
  klee_make_symbolic(&a[2], sizeof(a[2]), "a2");
  klee_make_symbolic(&a[3], sizeof(a[3]), "a3");
  klee_make_symbolic(&a[4], sizeof(a[4]), "a4");
  klee_make_symbolic(&a[5], sizeof(a[5]), "a5");

  gsl_vector_complex_view A = gsl_vector_complex_view_array(a, 6);

  CBLAS_INDEX_t index = gsl_blas_izamax(&A.vector);
  printf("%ld\n", index);

  return 0;
}
