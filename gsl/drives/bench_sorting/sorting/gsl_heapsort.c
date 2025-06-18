/*=====add by yx =====*/

//#include <stdio.h>
#include <gsl/gsl_heapsort.h>
#include <klee/klee.h>

int compare_doubles (const double * a, const double * b)
{
  if (*a > *b)
    return 1;
  else if (*a < *b)
    return -1;
  else
    return 0;
}

int main (void)
{
  int i;
  /* coefficients of P(x) =  -1 + x^5  */
  double a0,a1,a2,a3,a4,a5;
  //double a[6] = { -1, 5, 0, 0, 0, 1 };

  klee_make_symbolic(&a0, sizeof(a0), "a0");
  klee_make_symbolic(&a1, sizeof(a1), "a1");
  klee_make_symbolic(&a2, sizeof(a2), "a2");
  klee_make_symbolic(&a3, sizeof(a3), "a3");
  klee_make_symbolic(&a4, sizeof(a4), "a4");
  klee_make_symbolic(&a5, sizeof(a5), "a5");
  double a[6] = {a0, a1, a2, a3, a4, a5};

  gsl_heapsort (a, 6, sizeof(double), compare_doubles);
/*
  for (i = 0; i < 6; i++)
    {
      printf ("%lf ",array[i]);
    }
  printf("\n");
*/
  return 0;
}



