//
// Created by liukunlin on 2021/8/19.
//
#include "klee/klee.h"
#include "tridiag.h"
int main()
{
    size_t N=10;
    size_t d_stride=1;
    size_t o_stride=1;
    size_t b_stride=1;
    size_t x_stride=1;
    double diag[N*d_stride];
    double offdiag[N*o_stride];
    double b[N*b_stride];
    double x[N*x_stride];
    klee_make_symbolic(diag, sizeof(diag),"diag");
    klee_make_symbolic(offdiag, sizeof(offdiag),"offdiag");
    klee_make_symbolic(b, sizeof(b),"b");
    solve_cyc_tridiag(
            diag, d_stride,
            offdiag, o_stride,
            b, b_stride,
            x, x_stride,
            N);
}
