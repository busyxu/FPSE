#include <klee/klee.h>
#include <gsl/gsl_sort.h>
#include <gsl/gsl_vector_double.h>

void symbolize(double *s, int len) {
    char name[3] = {'a', 'a', 0};
    for (int i = 0; i < len; i++) {
        double *p = malloc(sizeof(double));
        name[1] = '0' + i;
        klee_make_symbolic(p, sizeof(double), name);
        s[i] = *p;
    }
}

int main(){
    double dest[8];
    gsl_vector *src_vector = gsl_vector_alloc(8);
    symbolize(src_vector->data, 8);
    int r = gsl_sort_vector_largest(dest, 8, src_vector);
    return 0;
}
