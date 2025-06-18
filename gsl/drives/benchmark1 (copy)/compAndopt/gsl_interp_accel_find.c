/*=====add by yx =====*/

#include <klee/klee.h>
#include <stdio.h>
#include <gsl/gsl_interp.h>
#include <stdio.h>

int
main (void)
{

  double x=1,y0=2,y1=4,y2=6;

  klee_make_symbolic(&x, sizeof(x), "x");
  klee_make_symbolic(&y0, sizeof(y0), "y0");
  klee_make_symbolic(&y1, sizeof(y1), "y1");
  klee_make_symbolic(&y2, sizeof(y2), "y2");

  double y[3] = {y0, y1, y2};

  gsl_interp_accel *a = gsl_interp_accel_alloc();

  size_t res = gsl_interp_accel_find(a, y, 3, x);

  printf("%ld\n",res);

  gsl_interp_accel_free(a);
  return 0;
}