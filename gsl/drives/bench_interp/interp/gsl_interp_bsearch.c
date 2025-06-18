/*=====add by yx =====*/

#include <klee/klee.h>
#include <stdio.h>
#include <gsl/gsl_interp.h>
#include <stdio.h>

int
main (void)
{

  double x=1,y0=2,y1=4,y2=6;
  size_t lo=0, hi=100;

  klee_make_symbolic(&x, sizeof(x), "x");
  klee_make_symbolic(&y0, sizeof(y0), "y0");
  klee_make_symbolic(&y1, sizeof(y1), "y1");
  klee_make_symbolic(&y2, sizeof(y2), "y2");

  double y[3] = {y0, y1, y2};

  size_t res = gsl_interp_bsearch(y, x, lo, hi);
//  printf("%ld\n",res);

  return 0;
}