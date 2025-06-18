/*=====add by yx =====*/

#include <klee/klee.h>
#include <stdio.h>
#include <gsl/gsl_multifit.h>

int main ()
{
  double X0=1,X1=2,X2=3,X3=4;
  double y0=1,y1=2;

  klee_make_symbolic(&X0, sizeof(X0), "X0");
  klee_make_symbolic(&X1, sizeof(X1), "X1");
  klee_make_symbolic(&X2, sizeof(X2), "X2");
  klee_make_symbolic(&X3, sizeof(X3), "X3");
  klee_make_symbolic(&y0, sizeof(y0), "y0");
  klee_make_symbolic(&y1, sizeof(y1), "y1");

  gsl_matrix *X = gsl_matrix_alloc(2,2);
  gsl_vector *y = gsl_vector_alloc(2);
  gsl_matrix *cov = gsl_matrix_alloc(2,2);
  gsl_vector *c = gsl_vector_alloc(2);
  gsl_multifit_robust_workspace * work = gsl_multifit_robust_alloc(gsl_multifit_robust_default, 2, 2);

  gsl_matrix_set (X, 0, 0, X0);
  gsl_matrix_set (X, 0, 1, X1);
  gsl_matrix_set (X, 1, 0, X2);
  gsl_matrix_set (X, 1, 1, X3);
  gsl_vector_set(y, 0, y0);
  gsl_vector_set(y, 1, y1);

  int status = gsl_multifit_robust(X, y, c, cov, work);
  printf("%d\n",status);

  gsl_multifit_robust_free(work);

  gsl_matrix_free (X);
  gsl_vector_free (y);
  gsl_matrix_free (cov);
  gsl_vector_free (c);

  return 0;
}