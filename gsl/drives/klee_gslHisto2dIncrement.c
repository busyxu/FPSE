#include <klee/klee.h>
#include <gsl/gsl_histogram2d.h>
#include <gsl/gsl_errno.h>

#define M 107
#define N 239
int main(){
    double a, b;
    klee_make_symbolic(&a, sizeof(a), "a");
    klee_make_symbolic(&b, sizeof(b), "b");
    //klee_assume(b>a);
    gsl_histogram2d * h = gsl_histogram2d_alloc(M, N);
    gsl_histogram2d_set_ranges_uniform (h, 0, 1, 0, 1);
    gsl_histogram2d_increment (h, a, b);
    return 0;
}
