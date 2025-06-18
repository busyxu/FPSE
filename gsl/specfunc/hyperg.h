/* specfunc/hyperg.h
 * 
 * Copyright (C) 1996, 1997, 1998, 1999, 2000 Gerard Jungman
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

/* Author:  G. Jungman */

/* Miscellaneous implementations of use
 * for evaluation of hypergeometric functions.
 */
#ifndef _HYPERG_H_
#define _HYPERG_H_

#include <gsl/gsl_sf_result.h>


/* Direct implementation of 1F1 series.
 */
int
gsl_sf_hyperg_1F1_series_e(const double a, const double b, const double x, gsl_sf_result * result);


/* Implementation of the 1F1 related to the
 * incomplete gamma function: 1F1(1,b,x), b >= 1.
 */
int
gsl_sf_hyperg_1F1_1_e(double b, double x, gsl_sf_result * result);


/* 1F1(1,b,x) for integer b >= 1
 */
int
gsl_sf_hyperg_1F1_1_int_e(int b, double x, gsl_sf_result * result);


/* Implementation of large b asymptotic.
 * [Bateman v. I, 6.13.3 (18)]
 * [Luke, The Special Functions and Their Approximations v. I, p. 129, 4.8 (4)]
 *
 * a^2 << b, |x|/|b| < 1 - delta
 */
int
gsl_sf_hyperg_1F1_large_b_e(const double a, const double b, const double x, gsl_sf_result * result);


/* Implementation of large b asymptotic.
 *
 * Assumes a > 0 is small, x > 0, and |x|<|b|.
 */
int
gsl_sf_hyperg_U_large_b_e(const double a, const double b, const double x,
                             gsl_sf_result * result,
                             double * ln_multiplier
                             );


/* Implementation of 2F0 asymptotic series.
 */
int
gsl_sf_hyperg_2F0_series_e(const double a, const double b, const double x, int n_trunc,
                              gsl_sf_result * result);


int hyperg_1F1_asymp_negx(const double a, const double b, const double x,
                      gsl_sf_result * result);
int hyperg_1F1_asymp_posx(const double a, const double b, const double x,
                      gsl_sf_result * result);
int hyperg_1F1_largebx(const double a, const double b, const double x,
                      gsl_sf_result * result);
int hyperg_1F1_large2bm4a(const double a, const double b, const double x,
                      gsl_sf_result * result);
int hyperg_1F1_luke(const double a, const double c, const double xin,
                      gsl_sf_result * result);
int hyperg_1F1_1_series(const double b, const double x,
                      gsl_sf_result * result);
int hyperg_1F1_1_int(const int b, const double x,
                      gsl_sf_result * result);
int hyperg_1F1_1(const double b, const double x,
                      gsl_sf_result * result);
int hyperg_1F1_renorm_b0(const double a, const double x,
                      gsl_sf_result * result);
int hyperg_1F1_CF1_p_ser(const double a, const double b, const double x,
                      double * result);
int hyperg_1F1_small_a_bgt0(const double a, const double b, const double x,
                      gsl_sf_result * result);
int hyperg_1F1_beps_bgt0(const double eps, const double b, const double x,
                      gsl_sf_result * result);
int hyperg_1F1_beq2a_pos(const double a, const double x,
                      gsl_sf_result * result);
int hyperg_1F1_ab_posint(const int a, const int b, const double x,
                      gsl_sf_result * result);
int hyperg_1F1_a_negint_poly(const int a, const double b, const double x,
                      gsl_sf_result * result);
int hyperg_1F1_a_negint_lag(const int a, const double b, const double x,
                      gsl_sf_result * result);
int hyperg_1F1_ab_negint(const int a, const int b, const double x,
                      gsl_sf_result * result);
int hyperg_1F1_U(const double a, const double b, const double x,
                      gsl_sf_result * result);
int hyperg_1F1_ab_pos(const double a, const double b, const double x,
                      gsl_sf_result * result);
int hyperg_1F1_ab_neg(const double a, const double b, const double x,
                      gsl_sf_result * result);


#endif /* !_HYPERG_H_ */
