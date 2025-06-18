#include <klee/klee.h>
#include <gsl/gsl_poly.h>

int main(){
    double x, y;
    double c[3];
    klee_make_symbolic(c, 3*sizeof(double), "c");
    klee_make_symbolic(&x, sizeof(x), "x");
    y = gsl_poly_eval(c, 3, x);
    return 0;
}
