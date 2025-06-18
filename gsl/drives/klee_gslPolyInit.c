#include <klee/klee.h>
#include <gsl/gsl_poly.h>

int main(){
    double xa[7];
    double ya[7];
    double dd[7];
    klee_make_symbolic(xa, 7 * sizeof(double), "xa");
    klee_make_symbolic(ya, 7 * sizeof(double), "ya");
    
    gsl_poly_dd_init (dd, xa, ya, 7);  
    return 0;
}
