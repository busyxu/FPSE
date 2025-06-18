#include <klee/klee.h>
#include <gsl/gsl_histogram2d.h>
#include <gsl/gsl_errno.h>

#define M 107
#define N 239
#define M1 17
#define N1 23
#define MR 10
#define NR 5

int main(){
  /*
  gsl_histogram2d * h = gsl_histogram2d_alloc (10, 10);
  gsl_histogram2d_set_ranges_uniform (h, 
                                      0.0, 1.0,
                                      0.0, 1.0);
  */
  double xmin, xmax, ymin, ymax;
  gsl_histogram2d * h = gsl_histogram2d_alloc (100, 100);
  klee_make_symbolic(&xmin, sizeof(xmin), "xmin");
  klee_make_symbolic(&xmax, sizeof(xmax), "xmax");
  klee_make_symbolic(&ymin, sizeof(ymin), "ymin");
  klee_make_symbolic(&ymax, sizeof(ymax), "ymax");
  klee_assume(xmax>xmin);
  klee_assume(ymax>ymin);
  gsl_histogram2d_set_ranges_uniform (h,
                                      xmin, xmax,
                                      ymin, ymax);
  return 0;
}
