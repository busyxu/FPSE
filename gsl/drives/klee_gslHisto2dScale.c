#include <klee/klee.h>
#include <gsl/gsl_histogram2d.h>
#include <gsl/gsl_errno.h>

#define M 107
#define N 239
#define M1 17
#define N1 23
#define MR 10
#define NR 5

int main(){
    double x;
    gsl_histogram2d *h3 = gsl_histogram2d_alloc (M, N);
    gsl_histogram2d_set_ranges_uniform (h3, 0, 1, 0, 10);
    klee_make_symbolic(&x, sizeof(x), "x");
    gsl_histogram2d_scale(h3, x); 
    return 0;
}
