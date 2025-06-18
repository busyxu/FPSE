#include <klee/klee.h>
#include <gsl/gsl_poly.h>

int main(){
    double x0, x1, x2;
    klee_make_symbolic(&x0, sizeof(x0), "x0");
    klee_make_symbolic(&x1, sizeof(x1), "x1");
    klee_make_symbolic(&x2, sizeof(x2), "x2");
    gsl_poly_solve_cubic(x0, x1, x2, &x0, &x1, &x2);
    return 0;
}
