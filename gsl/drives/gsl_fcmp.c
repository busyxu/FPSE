//
// Created by liukunlin on 2021/8/28.
//
#include "klee/klee.h"
#include "gsl/gsl_sys.h"
int main()
{
    double x1, x2, epsilon;
    klee_make_symbolic(&x1,sizeof(x1),"x1");
    klee_make_symbolic(&x2,sizeof(x2),"x2");
    klee_make_symbolic(&epsilon,sizeof(epsilon),"epsilon");
    gsl_fcmp (x1, x2,epsilon);
}