#include <klee/klee.h>
#include <gsl/gsl_statistics.h>

int main(){
    double data[10];
    klee_make_symbolic(data, 10 * sizeof(double), "data");
    double mean = gsl_stats_mean(data, 1, 10);
    double variance = gsl_stats_variance(data, 1, 10);
    double variance_m = gsl_stats_variance_m(data, 1, 10, mean);
    return 0;
}
