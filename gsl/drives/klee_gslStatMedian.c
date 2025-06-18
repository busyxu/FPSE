#include <klee/klee.h>
#include <gsl/gsl_statistics.h>
#include <gsl/gsl_math.h>

int main(){
    double data[10];
    klee_make_symbolic(data, 10 * sizeof(double), "data");
    double median  = gsl_stats_median(data, 1, 10);
    return 0;
}
