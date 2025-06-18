//
// Created by liukunlin on 2021/8/19.
//
#include "klee/klee.h"
#include "tridiag.h"
int main()
{
    size_t N=10;
    size_t d_stride=1;
    size_t a_stride=1;
    size_t b_stride=1;
    size_t r_stride=1;
    size_t x_stride=1;
    double diag[N*d_stride];
    double abovediag[N*a_stride];
    double belowdiag[N*b_stride];
    double rhs[N*r_stride];
    double x[N*x_stride];
    klee_make_symbolic(diag, sizeof(diag),"diag");
    klee_make_symbolic(abovediag, sizeof(abovediag),"abovediag");
    klee_make_symbolic(belowdiag, sizeof(belowdiag),"belowdiag");
    klee_make_symbolic(rhs, sizeof(rhs),"b");
    solve_cyc_tridiag_nonsym(
            diag, d_stride,
            abovediag, a_stride,
            belowdiag, b_stride,
            rhs, r_stride,
            x, x_stride,
            N);
}
