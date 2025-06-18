#include <klee/klee.h>
#include <gsl/gsl_poly.h>

int main(){
    double x0, x1, x2;
    double a, b, c;
    klee_make_symbolic(&a, sizeof(a), "a");
    klee_make_symbolic(&b, sizeof(b), "b");
    klee_make_symbolic(&c, sizeof(c), "c");
    int n = gsl_poly_solve_cubic (a, b, c, &x0, &x1, &x2);
    return 0;
}
