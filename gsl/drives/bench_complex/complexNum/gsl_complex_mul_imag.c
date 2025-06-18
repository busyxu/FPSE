

//#include <stdio.h>
#include <gsl/gsl_complex_math.h>
#include <klee/klee.h>

int main() {
  double x1,y1,y;
  klee_make_symbolic(&x1, sizeof(x1), "x1");
  klee_make_symbolic(&y1, sizeof(y1), "y1");
  klee_make_symbolic(&y, sizeof(y), "y");

  gsl_complex a = gsl_complex_rect(x1, y1);

  gsl_complex result = gsl_complex_mul_imag(a,y);
  //printf("%lf\n",result);
  return 0;
}



