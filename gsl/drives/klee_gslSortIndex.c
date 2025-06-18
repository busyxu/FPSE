#include <klee/klee.h>
#include <gsl/gsl_sort.h>

int main(){
    double data[10];
    size_t res[10];
    klee_make_symbolic(data, 10 * sizeof(double), "data");
    gsl_sort_index(res, data, 1, 10);
    return 0;
}
