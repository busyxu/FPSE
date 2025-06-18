/*=====add by yx =====*/

//#include <stdio.h>
#include <gsl/gsl_poly.h>
//#include <klee/klee.h>

int main() {
  double c1,c2,c3,c4,c5;
  double x;
  
//  klee_make_symbolic(&c1, sizeof(c1), "c1");
//  klee_make_symbolic(&c2, sizeof(c2), "c2");
//  klee_make_symbolic(&c3, sizeof(c3), "c3");
//  klee_make_symbolic(&c4, sizeof(c4), "c4");
//  klee_make_symbolic(&c5, sizeof(c5), "c5");
//  klee_make_symbolic(&x, sizeof(x), "x");
	
  double c[5] = {c1,c2,c3,c4,c5};
  double res[5] = {0};
  size_t lnres=5;
  int result = gsl_poly_eval_derivs(c, 5, x, res, lnres);
  //for(int i=0; i<5; i++) printf("%lf \n",res[i]);
  //printf("%d\n",result);
  return 0;
}



