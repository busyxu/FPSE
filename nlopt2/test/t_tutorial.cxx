#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <iomanip>
#include <nlopt.hpp>
//#include <nlopt.h>
//#define cnt 0

int a = 0;
double myfunc(unsigned n, const double *x, double *grad, void *data)
{
//  dim = 2;
  a++;
//  printf("double:\n");
//  std::cout<<x[0]<<" "<<x[1]<<" "<<fabs(x[0]-1)+fabs(x[1])<<"\n";

  return x[0]+x[1]+x[2]+x[3];
}

double myvconstraint1(unsigned n, const double *x, double *grad, void *data)
{

    double cv1 = (x[2]*x[2])*(-1) + 1;
    double cv2 = x[2]*x[2] + x[3]*x[3]*(-1) + 1;
    double cv3 = x[1]*x[1]*(-1) + x[2]*x[2];
    double cv4 = x[0]*(x[1]*x[1])*(-4) + x[0]*x[3]*x[3]*(-4) + x[0]*x[3]*x[3]*(-1) + x[1]*x[1]*2 + x[3]*x[3];

//    if(0<m && 0<v10 && 0<v9 && ((v8*v8)*(-1) + 1) == 0 && (v8*v8 + v9*v9*(-1) + 1) == 0 && (v10*v10*(-1) + v8*v8) ==0 && m*(v10*v10)*(-4) + m*v9*v9*(-4) + m*v9*v9*(-1) + v10*v10*2 + v9*v9 == 0){
//        printf("1111111111111111\n");
//    }

    return cv1;
}

double myvconstraint2(unsigned n, const double *x, double *grad, void *data)
{

    double cv1 = (x[2]*x[2])*(-1) + 1;
    double cv2 = x[2]*x[2] + x[3]*x[3]*(-1) + 1;
    double cv3 = x[1]*x[1]*(-1) + x[2]*x[2];
    double cv4 = x[0]*(x[1]*x[1])*(-4) + x[0]*x[3]*x[3]*(-4) + x[0]*x[3]*x[3]*(-1) + x[1]*x[1]*2 + x[3]*x[3];

//    if(0<m && 0<v10 && 0<v9 && ((v8*v8)*(-1) + 1) == 0 && (v8*v8 + v9*v9*(-1) + 1) == 0 && (v10*v10*(-1) + v8*v8) ==0 && m*(v10*v10)*(-4) + m*v9*v9*(-4) + m*v9*v9*(-1) + v10*v10*2 + v9*v9 == 0){
//        printf("1111111111111111\n");
//    }

    return cv2;
}

double myvconstraint3(unsigned n, const double *x, double *grad, void *data)
{

    double cv1 = (x[2]*x[2])*(-1) + 1;
    double cv2 = x[2]*x[2] + x[3]*x[3]*(-1) + 1;
    double cv3 = x[1]*x[1]*(-1) + x[2]*x[2];
    double cv4 = x[0]*(x[1]*x[1])*(-4) + x[0]*x[3]*x[3]*(-4) + x[0]*x[3]*x[3]*(-1) + x[1]*x[1]*2 + x[3]*x[3];

//    if(0<m && 0<v10 && 0<v9 && ((v8*v8)*(-1) + 1) == 0 && (v8*v8 + v9*v9*(-1) + 1) == 0 && (v10*v10*(-1) + v8*v8) ==0 && m*(v10*v10)*(-4) + m*v9*v9*(-4) + m*v9*v9*(-1) + v10*v10*2 + v9*v9 == 0){
//        printf("1111111111111111\n");
//    }

    return cv3;
}

double myvconstraint4(unsigned n, const double *x, double *grad, void *data)
{

    double cv1 = (x[2]*x[2])*(-1) + 1;
    double cv2 = x[2]*x[2] + x[3]*x[3]*(-1) + 1;
    double cv3 = x[1]*x[1]*(-1) + x[2]*x[2];
    double cv4 = x[0]*(x[1]*x[1])*(-4) + x[0]*x[3]*x[3]*(-4) + x[0]*x[3]*x[3]*(-1) + x[1]*x[1]*2 + x[3]*x[3];

//    if(0<m && 0<v10 && 0<v9 && ((v8*v8)*(-1) + 1) == 0 && (v8*v8 + v9*v9*(-1) + 1) == 0 && (v10*v10*(-1) + v8*v8) ==0 && m*(v10*v10)*(-4) + m*v9*v9*(-4) + m*v9*v9*(-1) + v10*v10*2 + v9*v9 == 0){
//        printf("1111111111111111\n");
//    }

    return cv4;
}


int main() {

//  nlopt::opt opt("LD_MMA", 2);
  nlopt_opt opt;
  opt = nlopt_create(NLOPT_GN_MOEA,4);

  double lb[4]={0,0,0,0};
  double ub[4]={100,100,100,100};
//  lb[0] = 0; lb[1] = 0, lb[0] = 0; lb[1] = 0;//lower bounds
  nlopt_set_lower_bounds(opt, lb);
  nlopt_set_upper_bounds(opt, ub);
  nlopt_set_min_objective(opt, myfunc, NULL);
//    nlopt_add_equality_constraint(opt, myvconstraint1, NULL, 1e-8);
//    nlopt_add_equality_constraint(opt, myvconstraint2, NULL, 1e-8);
//    nlopt_add_equality_constraint(opt, myvconstraint3, NULL, 1e-8);
//    nlopt_add_equality_constraint(opt, myvconstraint4, NULL, 1e-8);
//  std::vector<double> step_size_arr(2, 0.5);
//  nlopt_set_initial_step(opt, step_size_arr.data());
  nlopt_set_stopval(opt, 0);
//  nlopt_set_xtol_rel(opt, 1e-10);
  nlopt_set_maxeval(opt, 500000);
  nlopt_set_population(opt, 200);
//  nlopt_set_param(opt, "grad_dis", 1.0);

  double x[4] = {10,10,10,10};
//  x[0] = 1.234; x[1] = 5.678;//initial value
  double minf;

    nlopt_optimize(opt, x, &minf);

    printf("minf:%lf\n",minf);
    nlopt_destroy(opt);
    return 1;

//  try{
////      if (nlopt_optimize(opt, x, &minf) < 0) {
////          printf("nlopt failed!\n");
////      }
////      else {
////          printf("found minimum at f(%g,%g) = %0.10g\n", x[0], x[1], minf);
////      }
////    nlopt_optimize(opt, x, &minf);
////    printf("solution:\n");
////    for(int j=0; j<4; j++){
////      printf("%lf ",x[j]);
////    }
////    printf("\n");
//////    int nvar = 4;
////////    double *dval = malloc(sizeof(double) * nvar);
//////    double *dval = new double[nvar];
//////    for (int i = 0; i < nvar; i++) {
//////      uint8_t bytes[8];
//////      for (int j = 0; j < 8; j++) {
//////        bytes[j] = x[i*8+j];
//////      }
//////      double d;
//////      memcpy(&d, bytes, sizeof(double));
//////      dval[i] = d;
//////    }
//////    for(int i=0; i<nvar; i++){
//////      printf("%lf ",dval[i]);
//////    }
//////    printf("%lf\n",minf);
////////    std::cout << x[0]<<" "<<x[1]<<" "<<minf<<"\n";
////    std::cout<<a<<"\n";
//    nlopt_destroy(opt);
//    return 1;
//  }
//  catch(std::exception &e) {
//    std::cerr << "nlopt failed: " << e.what() << std::endl;
//    return EXIT_FAILURE;
//  }


}
