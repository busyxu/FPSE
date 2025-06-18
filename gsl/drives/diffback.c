#include <gsl/gsl_diff.h>
#include <klee/klee.h>

#define GSL_SQRT_DBL_EPSILON   1.4901161193847656e-08
#define GSL_FN_EVAL(F,x) (*((F)->function))(x,(F)->params)
int
diff_backward (const gsl_function * f,
                   double x, double *result, double *abserr)
{
  /* Construct a divided difference table with a fairly large step
     size to get a very rough estimate of f''.  Use this to estimate
     the step size which will minimize the error in calculating f'. */

  int i, k;
  double h = GSL_SQRT_DBL_EPSILON;
  double a[3], d[3], a2;

  /* Algorithm based on description on pg. 204 of Conte and de Boor
     (CdB) - coefficients of Newton form of polynomial of degree 2. */

  for (i = 0; i < 3; i++)
    {
      a[i] = x + (i - 2.0) * h;
      d[i] = GSL_FN_EVAL (f, a[i]);
    }

  for (k = 1; k < 4; k++)
    {
      for (i = 0; i < 3 - k; i++)
        {
          d[i] = (d[i + 1] - d[i]) / (a[i + k] - a[i]);
        }
    }

  /* Adapt procedure described on pg. 282 of CdB to find best value of
     step size. */

  a2 = fabs (d[0] + d[1] + d[2]);

  if (a2 < 100.0 * GSL_SQRT_DBL_EPSILON)
    {
      a2 = 100.0 * GSL_SQRT_DBL_EPSILON;
    }
  
  double h1 = GSL_SQRT_DBL_EPSILON / (2.0 * a2);
  //h = sqrt (GSL_SQRT_DBL_EPSILON / (2.0 * a2));
  double temp;
  klee_make_symbolic(&temp, sizeof(temp), "temp");
  klee_assert(temp * temp == h1);

  if (temp > 100.0 * GSL_SQRT_DBL_EPSILON)
    {
      h = 100.0 * GSL_SQRT_DBL_EPSILON;
    }

  *result = (GSL_FN_EVAL (f, x) - GSL_FN_EVAL (f, x - h)) / h;
  *abserr = fabs (10.0 * a2 * h);

  return 1;
}

double 
f1 (double x, void *params)
{
  if (x < 0.0){
      if(x + 100.0 > 0.0){
          if(x * 2.0 > -100.0){
              return x;
          }
          else if(x * 2.0 == -100.0){
              return x * 2.0;
          }
          else{
              return x / 2.0;
          }
      }
      else if(x / 2.0 > -100.0){
          if(x - x / 2.0 > -75.0){
              return 4.0 * x;
          }
          else if(x == 150.0){
              return 7.0 * x;
          }
          else{
              return x + 9.0;
          }
      }
      else if(x - 300.0 > -600.0){
          if(x * 2 > -500.0){
            return 9.0 * x + 2.0;
          }
          else{
            return x;
          }
      }
      else if(x * 3.0 > -1200.0){
          if(x * 2 == 700.0){
            return x * 100.0;
          }
          else{
            return 4 * x;
          }
      }
      else{
          return x * 10.0 + 9.0;
      }
  }

  else if (x == 0.0){
      return 0.0;
  }

  else{
      if(x < 100.0){
          if(x * 2.0 < 100.0){
              if(x * 3.0 < 60.0){
                      return x * x;
              }
              else if(x * 40.0 < 1600.0){
                      return x / 2.0 + x * 2.0;
              }
              else{
                  return x;
              }
          }
          else if(x + x / 2.0 < 120.0){
              if(x - 10 < 60.0){
                      return x * 9.0;
              }
              else{
                  return x;
              }
          }
          else{
            return x;
          }
      }
      else if(x < 200.0){
          if(x * 2.0 < 300.0){
                      return x + 1.0 * x;
          }
          else if(x * 150.0 == 22500.0){
              return 1.0 / x;
          }
          else{
              return 8.0 * x;
          }
      }
      else{
          return x;
      }
  }
}

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

int main(){
  gsl_function F;
  F.function = &f;
  double result, abserr, x;
  klee_make_symbolic(&x, sizeof(x), "x");
  diff_backward(&F, x, &result, &abserr);
  return 0;
}
