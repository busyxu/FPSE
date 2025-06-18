#include <klee/klee.h>
#include <gsl/gsl_statistics.h>

int main(){
    //int n;
    //klee_make_symbolic(&n, sizeof(n), "n");
    //klee_assume(n >= 0);
    double data[100];
    klee_make_symbolic(data, 100 * sizeof(double), "data");
    //klee_make_symbolic(&data, sizeof(data), "data");
    double mean;
    mean = gsl_stats_mean(data, 1, 100);
    return 0;
}
