#include <klee/klee.h>
#include <gsl/gsl_sort_vector_double.h>
#include <gsl/gsl_vector_double.h>

void symbolize(double *s, int len) {
    char name[3] = {'a', 'a', 0};
    for (int i = 0; i < len; i++) {
        double *p = malloc(sizeof(double));
        name[1] = 'a' + i;
        klee_make_symbolic(p, sizeof(double), name);
        s[i] = *p;
    }
}

int main(){
    double dest[10];
    gsl_vector *src_vector = gsl_vector_alloc(20);
    symbolize(src_vector->data, 20);
    int r = gsl_sort_vector_smallest(dest, 10, src_vector);
    return 0;
}
