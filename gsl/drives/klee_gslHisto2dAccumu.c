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
  
  gsl_histogram2d * h = gsl_histogram2d_alloc (10, 10);
 
  gsl_histogram2d_set_ranges_uniform (h, 
                                      1.0, 10.0,
                                      0.0, 1.0);
  double i, j, k;
  klee_make_symbolic(&i, sizeof(i), "i");
  klee_make_symbolic(&j, sizeof(j), "j");
  klee_make_symbolic(&k, sizeof(k), "k");
  klee_assume(i>1.0);
  klee_assume(i<10.0);
  klee_assume(j>0.0);
  klee_assume(j<1.0);
  gsl_histogram2d_accumulate (h, i, j, k);
  return 0;
}
