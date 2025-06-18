

//#include <stdio.h>
#include <gsl/gsl_poly.h>
#include <klee/klee.h>

int main() {
  double x1,x2,x3,y1,y2,y3,w1,w2,w3;
//  double xp=1;
  
  klee_make_symbolic(&x1, sizeof(x1), "x1");
  klee_make_symbolic(&x2, sizeof(x2), "x2");
  klee_make_symbolic(&x3, sizeof(x3), "x3");
  klee_make_symbolic(&y1, sizeof(y1), "y1");
  klee_make_symbolic(&y2, sizeof(y2), "y2");
  klee_make_symbolic(&y3, sizeof(y3), "y3");
  klee_make_symbolic(&w1, sizeof(w1), "w1");
  klee_make_symbolic(&w2, sizeof(w2), "w2");
  klee_make_symbolic(&w3, sizeof(w3), "w3");
  klee_make_symbolic(&xp, sizeof(xp), "xp");
 
  double xa[3] = {x1,x2,x3};
  double ya[3] = {y1,y2,y3};
  double w[3] = {w1,w2,w3};	
  double dd[3] = {0};
  gsl_poly_dd_init(dd, xa, ya, 3);
  double c[3] = {0};
  
  gsl_poly_dd_taylor(c, xp, dd, xa, 3, w);
  //double result2 = gsl_poly_dd_eval(dd, xa, 3, 4);
  //double result3 = gsl_poly_dd_eval(dd, xa, 3, 5);

  //for(int i=0; i<3; i++) printf("%lf \n",c[i]);
  return 0;
}



