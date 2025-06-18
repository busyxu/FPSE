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
  double xr[MR + 1] = { 0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0 };
  double yr[NR + 1] = { 90.0, 91.0, 92.0, 93.0, 94.0, 95.0 };
  size_t i;
  gsl_histogram2d *hr;
  hr = gsl_histogram2d_calloc_range (MR, NR, xr, yr);
  for (i = 0; i <= MR; i++)
  {
    hr->xrange[i] = 0.0;
  }

  for (i = 0; i <= NR; i++)
  {
    hr->yrange[i] = 0.0;
  }
  int status = gsl_histogram2d_set_ranges (hr, xr, MR + 1, yr, NR + 1);
  */  
  double xr[MR + 1];
  double yr[NR + 1];
  klee_make_symbolic(xr, (MR+1) * sizeof(double), "xr");
  klee_make_symbolic(yr, (NR+1) * sizeof(double), "yr");
  size_t i;
  gsl_histogram2d *hr;
  hr = gsl_histogram2d_calloc_range (MR, NR, xr, yr);
  for (i = 0; i <= MR; i++)
  {
    hr->xrange[i] = 0.0;
  }

  for (i = 0; i <= NR; i++)
  {
    hr->yrange[i] = 0.0;
  }
  int status = gsl_histogram2d_set_ranges (hr, xr, MR + 1, yr, NR + 1); 
  return 0;
}
