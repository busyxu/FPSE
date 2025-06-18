//
// Created by liukunlin on 2021/8/19.
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
    klee_make_symbolic(bin, sizeof(bin),"bin");
//    for(int i=1;i<nx+1;i++)
//    {
//        klee_assume(xrange[i-1]<xrange[i]);
//        klee_assume(yrange[i-1]<yrange[i]);
//    }
//    for(int i=0;i<nx*ny;i++)
//    {
//        klee_assume(bin[i]>0);
//    }
    gsl_histogram2d_pdf * p = gsl_histogram2d_pdf_alloc (nx, ny);
    gsl_histogram2d *h = gsl_histogram2d_alloc(nx,ny);
    gsl_histogram2d_set_ranges(h,xrange,nx+1,yrange,ny+1);
    h->bin = bin;
    gsl_histogram2d_pdf_init(p,h);
    double r1,r2;
    klee_make_symbolic(&r1, sizeof(r1),"r1");
    klee_make_symbolic(&r2, sizeof(r2),"r2");
    double x,y;
    gsl_histogram2d_pdf_sample (p,
                                r1, r2,
                                &x, &y);
}
