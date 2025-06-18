#include "klee/klee.h"
#include "gsl/gsl_sf_elljac.h"
int main()
{
    double u;
    klee_make_symbolic(&u,sizeof(u),"u");
    double m;
    klee_make_symbolic(&m,sizeof(m),"m");
    double sn = 0.0;
    double cn = 0.0;
    double dn = 0.0;
    
    gsl_sf_elljac_e(u, m, &sn, &cn, &dn);
}
