/* goSAT: automatically generated file */

#include "gofuncs.h"
#include <math.h>

#include <stdint.h>

double fp64_dis(const double a, const double b) {
    if (a == b || isnan(a) || isnan(b)) {
        return 0;
    }
    const double scale = pow(2, 54);
    uint64_t a_uint = *(const uint64_t *)(&a);
    uint64_t b_uint = *(const uint64_t *)(&b);
    if ((a_uint & 0x8000000000000000) != (b_uint & 0x8000000000000000)) {
        // signs are not equal return sum
        return ((double)
        ((a_uint & 0x7FFFFFFFFFFFFFFF) + (b_uint & 0x7FFFFFFFFFFFFFFF)))/scale;
    }
    b_uint &= 0x7FFFFFFFFFFFFFFF;
    a_uint &= 0x7FFFFFFFFFFFFFFF;
    if (a_uint < b_uint) {
        return ((double)(b_uint - a_uint))/scale;
    }
    return ((double)(a_uint - b_uint))/scale;
}

double (unsigned n, const double * x, double * grad, void * data){ 
const double expr_2798091491 = x[0] ;
const double expr_954062213 = 0;
const double expr_1586913185 = (expr_2798091491 >= expr_954062213)? 0: fp64_dis(expr_2798091491,expr_954062213);
const double expr_3157305753n = (expr_2798091491 != expr_954062213)? 0: 1 ;
const double expr_1168251917 = expr_3157305753n;
const double expr_783810685 = x[1] ;
const double expr_2730566147n = (expr_783810685 != expr_954062213)? 0: 1 ;
const double expr_1542123215 = expr_2730566147n;
const double expr_2831327593 = 1.000000 ;
const double expr_1924920005 = (expr_2798091491 <= expr_2831327593)? 0: fp64_dis(expr_2798091491,expr_2831327593);
const double expr_2883938295 = expr_1586913185 + expr_1168251917 + expr_1542123215 + expr_1924920005;
return expr_2883938295;
}




