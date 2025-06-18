#include <klee/klee.h>
#include <gsl/gsl_poly.h>

int main(){
    double x0, x1;
    double a, b, c;
    klee_make_symbolic(&a, sizeof(a), "a");
    klee_make_symbolic(&b, sizeof(b), "b");
    klee_make_symbolic(&c, sizeof(c), "c");
    int n = gsl_poly_solve_quadratic (a, b, c, &x0, &x1);
    return 0;
}
