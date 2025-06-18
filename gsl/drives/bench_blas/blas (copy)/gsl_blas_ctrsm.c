/*=====add by yx =====*/
#include <stdio.h>
#include <gsl/gsl_blas.h>
//#include <gsl/gsl_complex_math.h>
#include <klee/klee.h>

int main (void)
{
  
  //double c1,c2,c3  

  float a[8] = { 1, 0, 1, 0,1,0,1,0};

  float b[8] = { 1.2, 0.2, 1.5, 0.2,1.2,0.2,1.2,0.3};



  //float c[4] = { 0.00, 0.00, 0.00, 0.00 };

//  gsl_complex alpha = gsl_complex_rect(1.0,1.0);
//  gsl_complex beta = gsl_complex_rect(1.0,1.0);
  gsl_complex_float alpha = {{1.0,0.0}};
  //gsl_complex_float beta = {{1.0,0.0}};
  /*
  klee_make_symbolic(&a[0], sizeof(a[0]), "a0");
  klee_make_symbolic(&a[1], sizeof(a[1]), "a1");
  klee_make_symbolic(&a[2], sizeof(a[2]), "a2");
  klee_make_symbolic(&a[3], sizeof(a[3]), "a3");
  klee_make_symbolic(&a[4], sizeof(a[0]), "a4");
  klee_make_symbolic(&a[5], sizeof(a[1]), "a5");
  klee_make_symbolic(&a[6], sizeof(a[2]), "a6");
  klee_make_symbolic(&a[7], sizeof(a[3]), "a7");
  klee_make_symbolic(&b[0], sizeof(b[0]), "b0");
  klee_make_symbolic(&b[1], sizeof(b[1]), "b1");
  klee_make_symbolic(&b[2], sizeof(b[2]), "b2");
  klee_make_symbolic(&b[3], sizeof(b[3]), "b3");
  klee_make_symbolic(&b[4], sizeof(b[4]), "b4");
  klee_make_symbolic(&b[5], sizeof(b[5]), "b5");
  klee_make_symbolic(&b[6], sizeof(b[6]), "b6");
  klee_make_symbolic(&b[7], sizeof(b[7]), "b7");
*/
  gsl_matrix_complex_float_view A = gsl_matrix_complex_float_view_array(a, 2, 2);
  gsl_matrix_complex_float_view B = gsl_matrix_complex_float_view_array(b, 2, 2);

  /* Compute C = A B */

  gsl_blas_ctrsm(CblasLeft, CblasUpper, CblasNoTrans, CblasNonUnit,
                  alpha, &A.matrix, &B.matrix);

  for(int i=0; i<8; i++)
  printf ("%g, ", b[i]);
  printf ("\n");

  return 0;
}
