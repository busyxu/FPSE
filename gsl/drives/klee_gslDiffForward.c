#include <gsl/gsl_diff.h>
#include <klee/klee.h>

double
f (double x, void *params)
{
  return 2.0 * x + 3.0 * (x * x);
}

double
f2 (double x, void *params)
{
  if (x != 0.0)
    {
      return 6.0 * (x * x);
    }
  else
    {
      return 0.0;
    }
}

int main(){
  gsl_function F;
  F.function = &f2;
  double result, abserr, x;
  klee_make_symbolic(&x, sizeof(x), "x");
  gsl_diff_forward(&F, x, &result, &abserr);
  return 0;
}
