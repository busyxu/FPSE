//
// Created by liukunlin on 2021/8/28.
//
#include "klee/klee.h"
#include "gsl/gsl_histogram2d.h"

int main()
{
    size_t nx = 3;
    size_t ny = 3;
    double xrange[nx+1];
    double yrange[ny+1];
    double bin[nx*ny];
    klee_make_symbolic(xrange, sizeof(xrange),"xrange");
    klee_make_symbolic(yrange, sizeof(yrange),"yrange");
    gsl_histogram2d *h = gsl_histogram2d_alloc(nx,ny);
    gsl_histogram2d_set_ranges(h,xrange,nx+1,yrange,ny+1);
    double x,y;
    klee_make_symbolic(&x, sizeof(x),"x");
    klee_make_symbolic(&y, sizeof(y),"y");
    double weight;
    klee_make_symbolic(weight, sizeof(double),"weight");
    gsl_histogram2d_accumulate(h,x,y,weight);
}

