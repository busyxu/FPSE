//
// Created by liukunlin on 2021/8/29.
//
#include "klee/klee.h"
#include "gsl/gsl_bspline.h"
int main()
{
    double abserr;
    size_t nabscissae = 5;
    double abscissae_data[nabscissae];
    klee_make_symbolic(abscissae_data, sizeof(abscissae_data),"abscissae_data");
    gsl_vector_const_view abscissae
            = gsl_vector_const_view_array(abscissae_data, nabscissae);
    size_t k = 8;
    size_t nbreak = 5;
    gsl_bspline_workspace *w = gsl_bspline_alloc(k, nbreak);
    gsl_bspline_knots_greville(&abscissae.vector, w, &abserr);
}
