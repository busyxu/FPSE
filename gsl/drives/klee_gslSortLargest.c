#include <klee/klee.h>
#include <gsl/gsl_sort.h>

int main(){
    double dest[10], src[20];
    klee_make_symbolic(src, 20 * sizeof(double), "src");
    int r = gsl_sort_largest(dest, 10, src, 1, 20);
    return 0;
}
