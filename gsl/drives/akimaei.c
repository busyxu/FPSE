//
// Created by liukunlin on 2021/8/19.
//
#include "klee/klee.h"
#include "gsl/gsl_interp.h"

typedef struct
{
    double * b;
    double * c;
    double * d;
    double * _m;
} akima_state_t;

int main()
{
//    akima_state_t vstate;
    size_t size=5;
//    double state_b[size];
//    double state_c[size];
//    double state_d[size];
//    double state_m[size];
//    klee_make_symbolic(&state_b,sizeof(state_b),"state_b");
//    klee_make_symbolic(&state_c,sizeof(state_c),"state_c");
//    klee_make_symbolic(&state_d,sizeof(state_d),"state_d");
//    klee_make_symbolic(&state_m,sizeof(state_m),"state_m");
//    vstate.b = state_b;
//    vstate.c = state_c;
//    vstate.d = state_d;
//    vstate._m = state_m;
//    double x_array[5]={0.1,0.5,1.1,2.4,3.6};
//    double y_array[5]={0.8,1.7,2.2,3.1,3.5};
    double x_array[5];
    double y_array[5];
    klee_make_symbolic(x_array, sizeof(x_array),"x_array");
    klee_make_symbolic(y_array, sizeof(y_array),"y_array");
//    klee_assume(x_array[0]<x_array[1]);
//    klee_assume(x_array[1]<x_array[2]);
//    klee_assume(x_array[2]<x_array[3]);
//    klee_assume(x_array[3]<x_array[4]);
//    klee_make_symbolic(&x_array, sizeof(x_array),"x_array");
//    klee_make_symbolic(&y_array, sizeof(y_array),"y_array");
    gsl_interp_accel *acc = gsl_interp_accel_alloc ();
    gsl_interp *interp = gsl_interp_alloc(gsl_interp_akima,size);
//    size_t  cache;
//    size_t  miss_count;
//    size_t  hit_count;
//    klee_make_symbolic(&cache, sizeof(cache),"cache");
//    klee_make_symbolic(&miss_count, sizeof(miss_count),"miss_count");
//    klee_make_symbolic(&hit_count, sizeof(hit_count),"hit_count");
//    acc.cache = cache;
//    acc.miss_count = miss_count;
//    acc.hit_count = hit_count;
//    klee_make_symbolic(&acc,sizeof(acc),"acc");
    double a,b;
    klee_make_symbolic(&a,sizeof(a),"a");
    klee_make_symbolic(&b,sizeof(b),"b");

    double result;
    gsl_interp_init(interp,x_array,y_array,size);
    gsl_interp_eval_integ(interp,x_array, y_array,a, b,acc);
//    akima_eval_integ (&vstate,
//                      x_array, y_array, size,
//                      &acc, a, b,
//                      &result);
}
