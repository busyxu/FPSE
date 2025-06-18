#include <klee/klee.h>
#include <gsl/gsl_statistics.h>
#include <gsl/gsl_math.h>

int main(){
    double data[10];
    klee_make_symbolic(data, 10 * sizeof(double), "data");
    double largest  = gsl_stats_max(data, 1, 10);
    double smallest = gsl_stats_min(data, 1, 10);
    return 0;
}
