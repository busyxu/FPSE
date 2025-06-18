#include <klee/klee.h>
#include <gsl/gsl_sort.h>

int main(){
    double data1[10], data2[10];
    klee_make_symbolic(data1, 10 * sizeof(double), "data1");
    klee_make_symbolic(data2, 10 * sizeof(double), "data2");
    gsl_sort2(data1, 1, data2, 1, 10);
    return 0;
}
