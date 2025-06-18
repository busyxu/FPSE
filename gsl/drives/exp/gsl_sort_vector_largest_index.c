#include <klee/klee.h>
#include <gsl/gsl_vector_double.h>
#include <gsl/gsl_sort_vector_double.h>
void symbolize(gsl_vector *s, int len) {
    char name[3] = {'a', 'a', 0};
    for (int i = 0; i < len; i++) {
        double *p = malloc(sizeof(double));
        name[1] = 'a' + i;
        klee_make_symbolic(p, sizeof(double), name);
        gsl_vector_set(s,i,*p);
    }
}
int main(){
    size_t dest[10];
    gsl_vector *src_vector = gsl_vector_alloc(20);
//    double data[20];
//    symbolize(data,20);
//    src_vector->data = data;
    symbolize(src_vector,20);
    gsl_sort_vector_largest_index(dest, 10, src_vector);
    return 0;
}