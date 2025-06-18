//===------------------------------------------------------------*- C++ -*-===//
//
// This file is distributed under MIT License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// Copyright (c) 2017 University of Kaiserslautern.
//

#include "FPAUtils.h"
#include <cmath>
#include <cstring>
#include <limits>
#include <gmpxx.h>
#include <stdio.h>

/**
 * /brief Provides a scaled distance value between representations
 * of two double variables
 */
double fp64_dis(const double a, const double b)
{
    /*
     * Helpful. Ordered layout of FP32
     * 1 11111111 ----------------------- -nan
     * 1 11111111 00000000000000000000000 -oo
     * 1 -------- ----------------------- -norm
     * 1 00000000 ----------------------- -denorm
     * 1 00000000 00000000000000000000000 -o
     * 0 11111111 ----------------------- +nan
     * 0 11111111 00000000000000000000000 +oo
     * 0 -------- ----------------------- +norm
     * 0 00000000 ----------------------- +denorm
     * 0 00000000 00000000000000000000000 +o
     */
//  printf("dis:\n%lf\n%lf\n________\n",a,b);
    if (a == b) {
        return 0;
    }
    if ( std::isnan(a) || std::isnan(b)) {
        // any non-zero should do
        return 1024;
    }
    const double scale = pow(2, 54);
    //为什么要转成int, 是不是为了缩小决策空间,会不会影响找不到解
    uint64_t a_uint = *(const uint64_t*) (&a);
    uint64_t b_uint = *(const uint64_t*) (&b);
    if ((a_uint & 0x8000000000000000) != (b_uint & 0x8000000000000000)) {
        // signs are not equal return sum
        return ((double)
                ((a_uint & 0x7FFFFFFFFFFFFFFF) +
                 (b_uint & 0x7FFFFFFFFFFFFFFF))) / scale;
    }
    b_uint &= 0x7FFFFFFFFFFFFFFF;
    a_uint &= 0x7FFFFFFFFFFFFFFF;
    if (a_uint < b_uint) {
        return ((double) (b_uint - a_uint)) / scale;
    }
//  printf("%d\n%d\n________\n",a_uint,b_uint);
    return ((double) (a_uint - b_uint)) / scale;
//    printf("%lf\n%lf\n%lf\n________\n",a,b,fabs(a-b));
//    return (double) fabs(a-b);
}

double fp64_gt_dis(const double a, const double b)
{
//  printf("gt:\n%lf\n%lf\n",a,b);
  if(a>b){
    return 0;
  }
    if (a == b) {
        return DBL_MIN;
    }
  if ( std::isnan(a) || std::isnan(b)) {
    // any non-zero should do
    return 1024;
  }
  return fp64_dis(a, b);
}

double fp64_lt_dis(const double a, const double b)
{
//  printf("lt: %lf %lf\n",a,b);
  if (a < b) {//理论上，进入这个距离函数了，说明a>=b
    return 0;
  }
  if(a==b){
    return DBL_MIN;
  }
  if ( std::isnan(a) || std::isnan(b)) {
    // any non-zero should do
    return 1024;
  }
  return fp64_dis(a, b);
}

double fp64_ge_dis(const double a, const double b)
{

  if (a >= b) {
    return 0;
  }
  if ( std::isnan(a) || std::isnan(b)) {
    // any non-zero should do
    return 1024;
  }
    return fp64_dis(a, b);
}

double fp64_le_dis(const double a, const double b)
{
  /*
   * Helpful. Ordered layout of FP32
   * 1 11111111 ----------------------- -nan
   * 1 11111111 00000000000000000000000 -oo
   * 1 -------- ----------------------- -norm
   * 1 00000000 ----------------------- -denorm
   * 1 00000000 00000000000000000000000 -o
   * 0 11111111 ----------------------- +nan
   * 0 11111111 00000000000000000000000 +oo
   * 0 -------- ----------------------- +norm
   * 0 00000000 ----------------------- +denorm
   * 0 00000000 00000000000000000000000 +o
   */

  if (a <= b) {//理论上，进入这个距离函数了，说明a>=b
    return 0;
  }
  if ( std::isnan(a) || std::isnan(b)) {
    // any non-zero should do
    return 1024;
  }
//  return ((double) (a-b));
    return fp64_dis(a, b);

}

double fp64_eq_dis(const double a, const double b)
{
    if (a == b) {
        return 0;
    }

    return fp64_dis(a, b);
}

double fp64_neq_dis(const double a, const double b)
{
    if (a == 0 && b != 0) {
        return 0;
    }
    if (a != 0 && b == 0) {
        return 0;
    }
    if (a != b) {
        // it is possible that both sides are false, i.e. a != 0 && b != 0,
        // yet a == b and thus fp64_dis(a,b) would return 0 which is unsound.
        return 0;
    }
    return 1;
}

double fp64_isnan(const double a, const double flag)
{
    if (flag != 0) {
        // flag set, invert result
        return std::isnan(a)? 1.0: 0.0;
    } else {
        return std::isnan(a)? 0.0: 1.0;
    }
}

// add by zgfS
double fp64_isinf(const double a, const double flag)
{
  if (flag != 0) {
    // flag set, invert result
    return std::isinf(a)? 1.0: 0.0;
  } else {
    return std::isinf(a)? 0.0: 1.0;
  }
}
double fp64_ite(double flag, double a, double b) {
  return flag > 0 ? a : b;
}
double fp64_band(double a, double b) {
  uint64_t *au = (uint64_t*)&a, *bu = (uint64_t*)&b;
  uint64_t resu = *au & *bu;
  double *res = (double *)&resu;
  return *res;
}
double fp64_bor(double a, double b) {
  uint64_t *au = (uint64_t*)&a, *bu = (uint64_t*)&b;
  uint64_t resu = *au | *bu;
  double *res = (double *)&resu;
  return *res;
}
double fp64_bxor(double a, double b) {
  uint64_t *au = (uint64_t*)&a, *bu = (uint64_t*)&b;
  uint64_t resu = *au ^ *bu;
  double *res = (double *)&resu;
  return *res;
}
double fp64_tan(double a) {return std::tan(a);}
double fp64_asin(double a){return std::asin(a);}
double fp64_acos(double a){return std::acos(a);}
double fp64_atan(double a){return std::atan(a);}
double fp64_sinh(double a){return std::sinh(a);}
double fp64_cosh(double a){return std::cosh(a);}
double fp64_tanh(double a){return std::tanh(a);}
double fp64_pow(double a, double b){return std::pow(a,b);}
double fp64_atan2(double a, double b){return std::atan2(a,b);}
double fp64_fmin(double a, double b){return std::fmin(a,b);}
double fp64_fmax(double a, double b){return std::fmax(a,b);}

double fp64_overflow_dis(double op1, double op2, int opcode, int type)
{
  double dis = DBL_MAX;
  // GMP lib can not accept nan/inf
  if (std::isinf(op1) || std::isnan(op1) ||
      std::isinf(op2) || std::isnan(op2))
    dis =  DBL_MAX;
  if (opcode == 4 && op2==0){
    dis = DBL_MAX;
  }

//    long int_value1 = 0x7fefffffffffffff;
//    long int_value2 = 0x0000000000000001;
//    double *dmax = (double *)&int_value1;
//    double *dmin = (double *)&int_value2;

  mpq_t hVal1, hVal2, hRes, FMax, FMin, FZero, hResMax, hResMin;
  mpq_init(hVal1);mpq_init(hVal2);mpq_init(hRes);
  mpq_init(FMax);mpq_init(FMin);mpq_init(FZero);
  mpq_init(hResMax),mpq_init(hResMin);
  mpq_set_d(hVal1,op1);
  mpq_set_d(hVal2,op2);
  mpq_set_d(hRes,0);
  mpq_set_d(FZero,0);
  mpq_set_d(FMax,DBL_MAX);
  mpq_set_d(FMin,DBL_MIN);
//    mpq_set_d(FMax,*dmax);
//    mpq_set_d(FMin,*dmin);

  switch (opcode) {
    case 1: // opcode == 1 : FAdd
      mpq_add(hRes, hVal1, hVal2);
      mpq_abs(hRes, hRes);
      break;
    case 2: // opcode == 2 : FSub
      mpq_sub(hRes, hVal1, hVal2);
      mpq_abs(hRes, hRes);
      break;
    case 3: // opcode == 3 : FMul
      mpq_mul(hRes, hVal1, hVal2);
      mpq_abs(hRes, hRes);
      break;
    case 4:
      double absOp1 = fabs(op1);
      double absOp2 = fabs(op2);
      mpq_mul(hResMax, hVal2, FMax);
      mpq_mul(hResMin, hVal2, FMin);
      break;
  }

  // Note : mpf_cmp_d special use
  int overCmp = mpq_cmp(hRes,FMax);
  int underCmp1 = mpq_cmp(hRes,FZero);
  int underCmp2 = mpq_cmp(hRes,FMin);

  if(opcode==4){
    overCmp = mpq_cmp(hVal1,hResMax);
    underCmp1 = mpq_cmp(hVal1,FZero);
    underCmp2 = mpq_cmp(hVal1,hResMin);
  }

  // check Overflow
  if (type == 1){
    if (overCmp > 0)// hRes > DBL_MAX -> overCmp > 0
    {
      dis = 0.0;
    }
    else
      dis = 1.0-overCmp;
  }

  // check Underflow
  if (type == 2){
    if (0 < underCmp1 && underCmp2 < 0)
      return 0.0;
    else
      dis = (1.0-underCmp1) + (underCmp2 + 1.0);
  }

  mpq_clear(hVal1);mpq_clear(hVal2);mpq_clear(hRes);
  mpq_clear(FMax);mpq_clear(FMin);mpq_clear(FZero);

  return dis;
}

bool GMPEvaluateComm(double op1,double op2, int opcode, int type){
  // GMP lib can not accept nan/inf
  if (std::isinf(op1) || std::isnan(op1) ||
      std::isinf(op2) || std::isnan(op2))
    return false;

  bool ret = false;

    long int_value1 = 0x7fefffffffffffff;
    long int_value2 = 0x0000000000000001;
    double *dmax = (double *)&int_value1;
    double *dmin = (double *)&int_value2;
  mpq_t hVal1, hVal2, hRes, FMax, FMin, FZero;
  mpq_init(hVal1);mpq_init(hVal2);mpq_init(hRes);
  mpq_init(FMax);mpq_init(FMin);mpq_init(FZero);
  mpq_set_d(hVal1,op1);
  mpq_set_d(hVal2,op2);
  mpq_set_d(hRes,0);
  mpq_set_d(FZero,0);
  mpq_set_d(FMax,DBL_MAX);
  mpq_set_d(FMin,DBL_MIN);
//    mpq_set_d(FMax,*dmax);
//    mpq_set_d(FMin,*dmin);

  switch (opcode) {
    case 1: // opcode == 1 : FAdd
      mpq_add(hRes, hVal1, hVal2);
      mpq_abs(hRes, hRes);
      break;
    case 2: // opcode == 2 : FSub
      mpq_sub(hRes, hVal1, hVal2);
      mpq_abs(hRes, hRes);
      break;
    case 3: // opcode == 3 : FMul
      mpq_mul(hRes, hVal1, hVal2);
      mpq_abs(hRes, hRes);
      break;
  }

  // Note : mpf_cmp_d special use
  int overCmp = mpq_cmp(hRes,FMax);
  int underCmp1 = mpq_cmp(hRes,FZero);
  int underCmp2 = mpq_cmp(hRes,FMin);

  // check Overflow
  if (type == 1){
    if (overCmp > 0)// hRes > DBL_MAX -> overCmp > 0
    {
//      printf("overcmp>0;;;");
      ret = true;
    }
    else
      ret = false;
  }

  // check Underflow
  if (type == 2){
    if (0 < underCmp1 && underCmp2 < 0)
      ret = true;
    else
      ret = false;
  }

  mpq_clear(hVal1);mpq_clear(hVal2);mpq_clear(hRes);
  mpq_clear(FMax);mpq_clear(FMin);mpq_clear(FZero);

  // accuracy bug
  if(type==5){
    double add = op1+op2;
    if(add-op2 != op1 || add-op1 != op2){
      ret = true;
    }
    else ret = false;
  }
  if(type==6){
    double sub = op1-op2;
    if(sub+op2 != op1 || op1-sub != op2){
      ret = true;
    }
    else ret = false;
  }
  if(type==7){
    double mul = op1*op2;
    if((op2!=0 && mul/op2 != op1) || (op1!=0 && mul/op1 != op2)){
      return true;
    }
    else ret = false;
  }

  return ret;
}

bool GMPEvaluateFDiv(double op1,double op2, int type){
  // GMP lib can not accept nan/inf
  if (std::isinf(op1) || std::isnan(op1) ||
      std::isinf(op2) || std::isnan(op2))
    return false;

  if (type == 3){
    if (op1 == 0.0 && op2 == 0.0)
      return true;
    return false;
  }

  if (type == 4){
    bool r1 = op1 != 0.0;
    bool r2 = op2 == 0.0;
    if (op1 != 0.0 && op2 == 0.0)
      return true;
    return false;
  }
  bool ret = false;

  double absOp1 = fabs(op1);
  double absOp2 = fabs(op2);

    long int_value1 = 0x7fefffffffffffff;
    long int_value2 = 0x0000000000000001;
    double *dmax = (double *)&int_value1;
    double *dmin = (double *)&int_value2;
  mpq_t hVal1, hVal2, DMax, DMin, DZero, hResMax, hResMin;
  mpq_init(hVal1);mpq_init(hVal2);
  mpq_init(DMax);mpq_init(DMin);mpq_init(DZero);
  mpq_init(hResMax);mpq_init(hResMin);
  mpq_set_d(hVal1,absOp1);
  mpq_set_d(hVal2,absOp2);
  mpq_set_d(DMax,DBL_MAX);
  mpq_set_d(DMin,DBL_MIN);
//    mpq_set_d(DMax,*dmax);
//    mpq_set_d(DMin,*dmin);
  mpq_set_d(DZero,0.0);

  mpq_mul(hResMax, hVal2, DMax);
  mpq_mul(hResMin, hVal2, DMin);

  // Note : mpf_cmp_d special use
  int overCmp = mpq_cmp(hVal1,hResMax);
  int underCmp1 = mpq_cmp(hVal1,DZero);
  int underCmp2 = mpq_cmp(hVal1,hResMin);

  if (type == 1){
    if (overCmp > 0)// hRes > DBL_MAX -> overCmp > 0
      ret = true;
    else
      ret = false;
  }

  if (type == 2){
    if (0 < underCmp1 && underCmp2 < 0)
      ret = true;
    else
      ret = false;
  }
  mpq_clear(hVal1);mpq_clear(hVal2);
  mpq_clear(DMax);mpq_clear(DMin);mpq_clear(DZero);
  mpq_clear(hResMax);mpq_clear(hResMin);

  if(type==8){
    if(op2==0){
      ret=false;
    }
    else{
      double div = op1/op2;
      if((div*op2 != op1) || (div!=0 && op1/div != op2)){
        ret = true;
      }
      else ret = false;
    }
  }
  return ret;
}

//add by yx    1.sqrt; 2.log; 3.pow
bool GMPEvaluateFInvalid(double op1, int type){

  if (std::isinf(op1) || std::isnan(op1))
    return false;

  if (type == 1 && op1 < 0)
    return true;
  if (type == 2 && op1 < 0)
    return true;
  if (type == 3 && op1 == 0)//base == 0
    return true;

  return false;
}

double fpcheck_dis(double a, double b, int opcode, int mode){
  // opcode : 1 -> fadd ; 2 -> fsub ; 3 -> fmul ; 4 -> fdiv;  5 -> Invalid;
  // mode : 1 -> over ; 2 -> under ; 3 -> invalid ; 4 -> div-zero;
  // mode : 5 -> add-accuracy; 6 -> sub-accuracy; 7 -> mul-accuracy; 8 -> div-accuracy;
  // 1(5) -> sqrt  2(5) -> log  3(5) -> pow

//  printf("dec:\n%lf\n%lf\n",a,b);
  if (std::isinf(a) || std::isnan(a) ||
      std::isinf(b) || std::isnan(b))
    return 1024;

  double dis = DBL_MAX;
  if(opcode <= 3){
    // GMP lib can not accept nan/inf
      long int_value1 = 0x7fefffffffffffff;
      long int_value2 = 0x0000000000000001;
      double *dmax = (double *)&int_value1;
      double *dmin = (double *)&int_value2;
    mpq_t hVal1, hVal2, hRes, FMax, FMin, FZero;
    mpq_init(hVal1);mpq_init(hVal2);mpq_init(hRes);
    mpq_init(FMax);mpq_init(FMin);mpq_init(FZero);
    mpq_set_d(hVal1,a);
    mpq_set_d(hVal2,b);
    mpq_set_d(hRes,0);
    mpq_set_d(FZero,0);
    mpq_set_d(FMax,DBL_MAX);
    mpq_set_d(FMin,DBL_MIN);
//      mpq_set_d(FMax,*dmax);
//      mpq_set_d(FMin,*dmin);

    switch (opcode) {
      case 1: // opcode == 1 : FAdd
        mpq_add(hRes, hVal1, hVal2);
        mpq_abs(hRes, hRes);
        break;
      case 2: // opcode == 2 : FSub
        mpq_sub(hRes, hVal1, hVal2);
        mpq_abs(hRes, hRes);
        break;
      case 3: // opcode == 3 : FMul
        mpq_mul(hRes, hVal1, hVal2);
        mpq_abs(hRes, hRes);
        break;
    }

    // Note : mpf_cmp_d special use
    int overCmp = mpq_cmp(hRes,FMax);
    int underCmp1 = mpq_cmp(hRes,FZero);
    int underCmp2 = mpq_cmp(hRes,FMin);

    // check Overflow
    if (mode == 1){
      if (overCmp > 0)// hRes > DBL_MAX -> overCmp > 0
      {
        dis = 0.0;
      }
      else
        dis = 1.0-overCmp;
    }

    // check Underflow
    if (mode == 2){
      if (0 < underCmp1 && underCmp2 < 0)
        return 0.0;
      else
        dis = (1.0-underCmp1) + (underCmp2 + 1.0);
    }

    mpq_clear(hVal1);mpq_clear(hVal2);mpq_clear(hRes);
    mpq_clear(FMax);mpq_clear(FMin);mpq_clear(FZero);

    if(opcode==1 && mode == 5){
      double add = a + b;
      dis = fmin(fp64_neq_dis(add-a, b), fp64_neq_dis(add-b,a));
//      return res;
    }
    if(opcode==2 && mode == 6){
      double sub = a - b;
      dis = fmin(fp64_neq_dis(sub+b, a), fp64_neq_dis(a-sub,b));
//      return res;
    }
    if(opcode==3 && mode == 7){
      double mul = a * b;
      dis = fmin(fp64_neq_dis(a,0)+fp64_neq_dis(mul/a, b), fp64_neq_dis(b,0)+fp64_neq_dis(mul/b,a));
//      return res;
    }
  }else if(opcode==4) {

    double absOp1 = fabs(a);
    double absOp2 = fabs(b);

    mpq_t hVal1, hVal2, DMax, DMin, DZero, hResMax, hResMin;
      long int_value1 = 0x7fefffffffffffff;
      long int_value2 = 0x0000000000000001;
      double *dmax = (double *)&int_value1;
      double *dmin = (double *)&int_value2;
    mpq_init(hVal1);mpq_init(hVal2);
    mpq_init(DMax);mpq_init(DMin);mpq_init(DZero);
    mpq_init(hResMax);mpq_init(hResMin);
    mpq_set_d(hVal1,absOp1);
    mpq_set_d(hVal2,absOp2);
    mpq_set_d(DMax,DBL_MAX);
    mpq_set_d(DMin,DBL_MIN);
//      mpq_set_d(DMax,*dmax);
//      mpq_set_d(DMin,*dmin);
    mpq_set_d(DZero,0.0);

    mpq_mul(hResMax, hVal2, DMax);
    mpq_mul(hResMin, hVal2, DMin);

    // Note : mpf_cmp_d special use
    int overCmp = mpq_cmp(hVal1,hResMax);
    int underCmp1 = mpq_cmp(hVal1,DZero);
    int underCmp2 = mpq_cmp(hVal1,hResMin);

    if (mode == 1){
      if (overCmp > 0)// hRes > DBL_MAX -> overCmp > 0
      {
        dis = 0.0;
      }
      else
        dis = 1.0-overCmp;
    }
    // check Underflow
    if (mode == 2){
      if (0 < underCmp1 && underCmp2 < 0)
        dis = 0.0;
      else
        dis = (1.0-underCmp1) + (underCmp2 + 1.0);
    }

    mpq_clear(hVal1);mpq_clear(hVal2);
    mpq_clear(DMax);mpq_clear(DMin);mpq_clear(DZero);
    mpq_clear(hResMax);mpq_clear(hResMin);

    if (mode == 3)
      dis = fp64_dis(a,0) + fp64_dis(b,0);
    if (mode == 4){
      dis = fp64_neq_dis(a,0) + fp64_dis(b,0);
    }
    if(mode == 8){
      if(b==0) return 1024;
      double div = a/b;
      dis = fmin(fp64_neq_dis(div*b, a), fp64_neq_dis(div, 0) + fp64_neq_dis(a/div, b));
//      return res;
    }
  }else{
    //need by yx
    if (mode == 1)
      dis = fp64_lt_dis(a,0);//sqrt
    if (mode == 2){
      dis = fp64_le_dis(a,0);//log
    }
    if (mode == 3)
      dis = fp64_neq_dis(a,0);//pow
  }
//  printf("dec:\n%lf\n%lf\n%lf\n",a,b,dis);
    return dis;
}

bool FPCHECK_FADD_OVERFLOW(const double left,const double right){
  return GMPEvaluateComm(left, right, 1, 1) ;
}
bool FPCHECK_FADD_UNDERFLOW(const double left,const double right){
  return GMPEvaluateComm(left, right, 1, 2) ;
}
bool FPCHECK_FSUB_OVERFLOW(const double left,const double right){
  return GMPEvaluateComm(left, right, 2, 1) ;
}
bool FPCHECK_FSUB_UNDERFLOW(const double left,const double right){
  return GMPEvaluateComm(left, right, 2, 2) ;
}
bool FPCHECK_FMUL_OVERFLOW(const double left,const double right){
  return GMPEvaluateComm(left, right, 3, 1) ;
}
bool FPCHECK_FMUL_UNDERFLOW(const double left,const double right){
  return GMPEvaluateComm(left, right, 3, 2);
}
bool FPCHECK_FDIV_OVERFLOW(const double left,const double right){
  return GMPEvaluateFDiv(left, right, 1) ;
}
bool FPCHECK_FDIV_UNDERFLOW(const double left,const double right){
  return GMPEvaluateFDiv(left, right, 2) ;
}
bool FPCHECK_FDIV_INVALID(const double left,const double right){
  return GMPEvaluateFDiv(left, right, 3);
}
bool FPCHECK_FDIV_ZERO(const double left,const double right){
  return GMPEvaluateFDiv(left, right, 4) ;
}
// add by yx
bool FPCHECK_FINVALID_SQRT(const double left,const double right){//right is placeholder
  return GMPEvaluateFInvalid(left, 1) ;
}
bool FPCHECK_FINVALID_LOG(const double left,const double right){//right is placeholder
  return GMPEvaluateFInvalid(left, 2) ;
}
bool FPCHECK_FINVALID_POW(const double left,const double right){//right is placeholder
  return GMPEvaluateFInvalid(left, 3) ;
}

bool FPCHECK_FADD_ACCURACY(const double left,const double right){
  return GMPEvaluateComm(left, right, 1, 5) ;
}
bool FPCHECK_FSUB_ACCURACY(const double left,const double right){
  return GMPEvaluateComm(left, right, 1, 6) ;
}
bool FPCHECK_FMUL_ACCURACY(const double left,const double right){
  return GMPEvaluateComm(left, right, 1, 7) ;
}
bool FPCHECK_FDIV_ACCURACY(const double left,const double right){
  return GMPEvaluateFDiv(left, right, 8) ;
}

namespace gosat {
namespace fpa_util {

bool isRoundingModeApp(const z3::expr expr) noexcept
{
    if (expr.num_args() != 0) {
        return false;
    }
    if (expr.decl().decl_kind() == Z3_OP_FPA_RM_NEAREST_TIES_TO_EVEN ||
        expr.decl().decl_kind() == Z3_OP_FPA_RM_NEAREST_TIES_TO_AWAY ||
        expr.decl().decl_kind() == Z3_OP_FPA_RM_TOWARD_POSITIVE ||
        expr.decl().decl_kind() == Z3_OP_FPA_RM_TOWARD_NEGATIVE ||
        expr.decl().decl_kind() == Z3_OP_FPA_RM_TOWARD_ZERO) {
        return true;
    }
    return false;
}

bool isBoolExpr(const z3::expr& expr) noexcept
{
    switch (expr.decl().decl_kind()) {
        case Z3_OP_TRUE:
        case Z3_OP_FALSE:
        case Z3_OP_EQ:
        case Z3_OP_FPA_EQ:
        case Z3_OP_NOT:
        case Z3_OP_AND:
        case Z3_OP_OR:
            // Floating point operations
        case Z3_OP_FPA_LT:
        case Z3_OP_FPA_GT:
        case Z3_OP_FPA_LE:
        case Z3_OP_FPA_GE:
        case Z3_OP_FPA_IS_NAN:
        case Z3_OP_FPA_IS_INF:
        case Z3_OP_FPA_IS_ZERO:
        case Z3_OP_FPA_IS_NORMAL:
        case Z3_OP_FPA_IS_SUBNORMAL:
        case Z3_OP_FPA_IS_NEGATIVE:
        case Z3_OP_FPA_IS_POSITIVE:
            return true;
        default:
            return false;
    }
}

bool
isFloat32VarDecl(const z3::expr& expr) noexcept
{
    unsigned sigd = Z3_fpa_get_sbits(expr.ctx(), expr.get_sort());
    unsigned expo = Z3_fpa_get_ebits(expr.ctx(), expr.get_sort());
    return isFloat32(expo, sigd)? true: false;
}

// add by zgf
bool isBV32VarDecl(const z3::expr& expr) noexcept {
    unsigned width = Z3_get_bv_sort_size(expr.ctx(), expr.get_sort());
    return isBV32(width);
}
bool isBV64VarDecl(const z3::expr& expr) noexcept {
    unsigned width = Z3_get_bv_sort_size(expr.ctx(), expr.get_sort());
    return isBV64(width);
}

bool
isFloat64VarDecl(const z3::expr& expr) noexcept
{
    unsigned sigd = Z3_fpa_get_sbits(expr.ctx(), expr.get_sort());
    unsigned expo = Z3_fpa_get_ebits(expr.ctx(), expr.get_sort());
    return isFloat64(expo, sigd)? true: false;
}

bool
isNonLinearFPExpr(const z3::expr& expr) noexcept
{
    if (expr.get_sort().sort_kind() != Z3_FLOATING_POINT_SORT) {
        return false;
    }
    switch (expr.decl().decl_kind()) {
        case Z3_OP_FPA_MUL:
        case Z3_OP_FPA_DIV:
        case Z3_OP_FPA_REM:
        case Z3_OP_FPA_ABS:
        case Z3_OP_FPA_MIN:
        case Z3_OP_FPA_MAX:
        case Z3_OP_FPA_FMA:
        case Z3_OP_FPA_SQRT:
        case Z3_OP_FPA_ROUND_TO_INTEGRAL:
            return true;
        default:
            return false;
    }
}

inline unsigned short
getBaseofNumStr(const char* numerical) noexcept
{
    if (numerical[1] == 'b') { return 2; }
    if (numerical[1] == 'x') { return 16; }
    if (numerical[1] == 'o') { return 8; }
    return 0;
}

/**
 *
 * @param expr
 * @return float value of expression
 * @pre isFloat32
 */
float
toFloat32(const z3::expr& expr) noexcept
{
    switch (expr.decl().decl_kind()) {
        case Z3_OP_FPA_PLUS_INF:
            return std::numeric_limits<float>::infinity();
        case Z3_OP_FPA_MINUS_INF:
            return -(std::numeric_limits<float>::infinity());
        case Z3_OP_FPA_NAN:
            return std::numeric_limits<float>::quiet_NaN();
        case Z3_OP_FPA_PLUS_ZERO:
            return 0;
        case Z3_OP_FPA_MINUS_ZERO:
            return -0;
        default:
            break;
    }
    // XXX: proper functions are not working as of Z3 v4.5.0
    //  - Z3_fpa_get_numeral_sign(expr.ctx(), static_cast<z3::ast>(expr), &sign);
    //  - Z3_fpa_get_numeral_exponent_int64(expr.ctx(), static_cast<z3::ast>(expr), &exponent);
    //  - Z3_fpa_get_numeral_significand_uint64(expr.ctx(), static_cast<z3::ast>(expr), &significand);
    assert(expr.num_args() == 3 && "<toFloat32> Invalid FP constant");
    Z3_string bv_str = Z3_ast_to_string(expr.ctx(),
                                        static_cast<z3::ast >(expr.arg(0)));
    int sign = std::stoi(bv_str + 2, NULL, getBaseofNumStr(bv_str));
    bv_str = Z3_ast_to_string(expr.ctx(),
                              static_cast<Z3_ast >(expr.arg(1)));
    uint64_t exponent = std::stoul(bv_str + 2, NULL, getBaseofNumStr(bv_str));
    bv_str = Z3_ast_to_string(expr.ctx(),
                              static_cast<z3::ast >(expr.arg(2)));
    uint64_t significand = std::stoull(bv_str + 2, NULL,
                                       getBaseofNumStr(bv_str));
    uint32_t result = static_cast<uint32_t >(exponent);
    result <<= 23;
    result |= static_cast<uint32_t >(significand);
    if (sign) result |= 0x80000000;
    return *(reinterpret_cast<float*>(&result));
}

/**
 *
 * @param exp
 * @return double value of expression
 * @pre isFloat64
 */
double
toFloat64(const z3::expr& expr) noexcept
{
    switch (expr.decl().decl_kind()) {
        case Z3_OP_FPA_PLUS_INF:
            return std::numeric_limits<double>::infinity();
        case Z3_OP_FPA_MINUS_INF:
            return -(std::numeric_limits<double>::infinity());
        case Z3_OP_FPA_NAN:
            return std::numeric_limits<double>::quiet_NaN();
        case Z3_OP_FPA_PLUS_ZERO:
            return 0;
        case Z3_OP_FPA_MINUS_ZERO:
            return -0;
        default:
            break;
    }
    assert(expr.num_args() == 3 && "<toFloat64> Invalid FP constant");
    Z3_string bv_str = Z3_ast_to_string(expr.ctx(),
                                        static_cast<z3::ast >(expr.arg(0)));
    int sign = std::stoi(bv_str + 2, NULL, getBaseofNumStr(bv_str));
    bv_str = Z3_ast_to_string(expr.ctx(),
                              static_cast<Z3_ast >(expr.arg(1)));
    uint64_t exponent = std::stoul(bv_str + 2, NULL, getBaseofNumStr(bv_str));
    bv_str = Z3_ast_to_string(expr.ctx(),
                              static_cast<z3::ast >(expr.arg(2)));
    uint64_t significand = std::stoull(bv_str + 2, NULL,
                                       getBaseofNumStr(bv_str));
    // Hidden bit must not be represented see:
    // http://smtlib.cs.uiowa.edu/theories-FloatingPoint.shtml
    uint64_t result = exponent;
    result <<= 52;
    result |= significand;
    if (sign) result |= 0x8000000000000000;
    return *(reinterpret_cast<double*>(&result));
}
}
}
