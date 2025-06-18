// ======== add by yx ========
#include <stdio.h>
#include <gsl/gsl_blas.h>
//#include <klee/klee.h>

int main (void)
{
  double a0=1, a1=0, a2=1, a3=0;
  double b0=1011, b1=1012, b2=1021, b3=0.11;
  double alpha0=1,alpha1=0;
//  klee_make_symbolic(&alpha0, sizeof(alpha0), "alpha0");
//  klee_make_symbolic(&alpha1, sizeof(alpha1), "alpha1");

//  klee_make_symbolic(&a0, sizeof(a0), "a0");
//  klee_make_symbolic(&a1, sizeof(a1), "a1");
//  klee_make_symbolic(&a2, sizeof(a2), "a2");
//  klee_make_symbolic(&a3, sizeof(a3), "a3");
//  klee_make_symbolic(&b0, sizeof(b0), "b0");
//  klee_make_symbolic(&b1, sizeof(b1), "b1");
//  klee_make_symbolic(&b2, sizeof(b2), "b2");
//  klee_make_symbolic(&b3, sizeof(b3), "b3");

  double a[] = { a0, a1, a2, a3};
  double b[] = { b0, b1, b2, b3};

  gsl_complex alpha = {{alpha0, alpha1}};

  gsl_vector_complex_view A = gsl_vector_complex_view_array(a, 2);
  gsl_vector_complex_view B = gsl_vector_complex_view_array(b, 2);

  int status = gsl_blas_zaxpy(alpha, &A.vector, &B.vector);
  printf("%d\n", status);
  for(int i=0; i<8; i++)
    printf ("%g, ", b[i]);
  printf ("\n");
  
  return 0;
}
