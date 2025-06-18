//
// Created by liukunlin on 2021/8/19.
//
#include "klee/klee.h"
#include "gsl/gsl_interp2d.h"
int main()
{
    size_t xsize=10;
    size_t ysize=8;
//    double xarr[10] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0};
//    double yarr[8] = {1.0, 4.0, 6.0, 8.0, 10.0, 12.0, 14.0, 16.0};
//    double zarr[80] = { 1,  2,  3,  4,  5,  6,  7,  8, 9, 10,
//                      2,  2,  6,  4, 10,  6, 14,  8, 11, 12,
//                      3,  6,  3, 12, 15,  6, 21, 24, 13, 14,
//                      4,  4, 12,  4, 20, 12, 28,  8, 15, 16,
//                      5, 10, 15, 20,  5, 30, 35, 40, 17, 18,
//                      6,  6,  6, 12, 30,  6, 42, 24, 19, 20,
//                      7, 14, 21, 28, 35, 42,  7, 56, 21, 22,
//                      8,  8, 24,  8, 40, 24, 56,  8, 23, 24};
    double xarr[xsize];
    double yarr[ysize];
    double zarr[xsize*ysize];
    klee_make_symbolic(xarr, sizeof(xarr),"xarr");
    klee_make_symbolic(yarr, sizeof(yarr),"yarr");
    klee_make_symbolic(zarr, sizeof(zarr),"zarr");
    gsl_interp2d *interp = gsl_interp2d_alloc(gsl_interp2d_bilinear,xsize,ysize);
//    for(int i=1;i<xsize;i++)
//    {
//        klee_assume(xarr[i-1]<xarr[i]);
//    }
//    for(int i=1;i<ysize;i++)
//    {
//        klee_assume(yarr[i-1]<yarr[i]);
//    }
    gsl_interp2d_init(interp,xarr,yarr,zarr,xsize,ysize);
    double x,y;
    klee_make_symbolic(&x, sizeof(x),"x");
    klee_make_symbolic(&y, sizeof(y),"y");
    klee_assume(x >= interp->xmin);
    klee_assume(x <= interp->xmax);
    klee_assume(y >= interp->ymin);
    klee_assume(y <= interp->ymax);
    gsl_interp_accel * xa = gsl_interp_accel_alloc();
    gsl_interp_accel * ya = gsl_interp_accel_alloc();
    gsl_interp2d_eval_deriv_y (interp,xarr,
                               yarr, zarr,
                               x, y,
                               xa, ya);
}
