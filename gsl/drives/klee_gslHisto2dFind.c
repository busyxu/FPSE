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
    double xpos, ypos;
    size_t xbin;
    size_t ybin;
    gsl_histogram2d *h3 = gsl_histogram2d_alloc (M, N);
    gsl_histogram2d_set_ranges_uniform (h3, 0, 1, 0, 10);
    klee_make_symbolic(&xpos, sizeof(xpos), "xpos");
    klee_make_symbolic(&ypos, sizeof(ypos), "ypos");
    klee_assume(xpos>0);
    klee_assume(xpos<1);
    klee_assume(ypos>0);
    klee_assume(ypos<10);
    gsl_histogram2d_find (h3, xpos, ypos, &xbin, &ybin); 
    return 0;
}
