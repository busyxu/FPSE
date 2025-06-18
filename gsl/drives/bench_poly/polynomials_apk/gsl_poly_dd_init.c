/*=====add by yx =====*/

//#include <stdio.h>
#include <gsl/gsl_poly.h>
//#include <klee/klee.h>

int main() {
  double x1,x2,x3,y1,y2,y3;
  //double x;
  
//  klee_make_symbolic(&x1, sizeof(x1), "x1");
//  klee_make_symbolic(&x2, sizeof(x2), "x2");
//  klee_make_symbolic(&x3, sizeof(x3), "x3");
//  klee_make_symbolic(&y1, sizeof(y1), "y1");
//  klee_make_symbolic(&y2, sizeof(y2), "y2");
//  klee_make_symbolic(&y3, sizeof(y3), "y3");
//  //klee_make_symbolic(&x, sizeof(x), "x");
	
  double xa[3] = {x1,x2,x3};
  double ya[3] = {y1,y2,y3};
  double dd[3] = {0};
  int status = gsl_poly_dd_init(dd, xa, ya, 3);

  //double result1 = gsl_poly_dd_eval(dd, xa, 3, x);
  //double result2 = gsl_poly_dd_eval(dd, xa, 3, 4);
  //double result3 = gsl_poly_dd_eval(dd, xa, 3, 5);

  //for(int i=0; i<3; i++) printf("%lf \n",dd[i]);
  //printf("%lf %lf %lf\n",result1, result2, result3);
  return 0;
}



