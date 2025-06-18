#include <klee/klee.h>
#include <gsl/gsl_sort.h>

int main(){
    size_t dest[10];
    double src[20];
    klee_make_symbolic(src, 20 * sizeof(double), "src");
    int r = gsl_sort_largest_index(dest, 10, src, 1, 20);
    return 0;
}
