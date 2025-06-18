#include <gsl/gsl_diff.h>
#include <klee/klee.h>

double
f (double x, void *params)
{
  if(x < 100.0){
    return x;
  }
  else if(x - 100.0 < 100.0){
    return 2.0 * x;
  }
  else if(x + 50.0 < 350.0){
    return x + 10.0;
  }
  else if(x * 2.0 < 800.0){
    return x / 1.0;
  }
  else if(x * 2.0 < 1000.0){
    return x + 10.0;
  }
  else if(x + x / 2.0 < 900.0){
    return 10.0 * x;
  }
  else if(x - 700.0 < 1000.0){
    return x;
  }
  else if(x * 2.0 - 600.0 < 1000.0){
    return x / 2.0;
  }
  else{
    return x;
  }
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
  F.function = &f;
  double result, abserr, x;
  klee_make_symbolic(&x, sizeof(x), "x");
  gsl_diff_backward(&F, x, &result, &abserr);
  return 0;
}
