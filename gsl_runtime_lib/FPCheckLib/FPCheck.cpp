// add by zgf to support GMP lib for FPCheck
#include <gmpxx.h>
#include <float.h>
#include <math.h>

bool GMPEvaluateComm(double op1,double op2, int opcode, int type){
  // GMP lib can not accept nan/inf
  if (std::isinf(op1) || std::isnan(op1) ||
      std::isinf(op2) || std::isnan(op2))
    return false;

  bool ret = false;
  mpq_t hVal1, hVal2, hRes, FMax, FMin, FZero;
  mpq_init(hVal1);mpq_init(hVal2);mpq_init(hRes);
  mpq_init(FMax);mpq_init(FMin);mpq_init(FZero);
  mpq_set_d(hVal1,op1);
  mpq_set_d(hVal2,op2);
  mpq_set_d(hRes,0);
  mpq_set_d(FZero,0);
/*	
  long int_value1 = 0x7fefffffffffffff;
  long int_value2 = 0x0000000000000001;
  double *dmax = (double *)&int_value1;
  double *dmin = (double *)&int_value2;
  mpq_set_d(FMax,*dmax);
  mpq_set_d(FMin,*dmin);
*/  
  mpq_set_d(FMax,DBL_MAX);
  mpq_set_d(FMin,DBL_MIN);

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
      ret = true;
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
  if(opcode==1 && type==5){
    double add = op1+op2;
    if(add-op2 != op1 || add-op1 != op2){
      ret = true;
    }
    else ret = false;
  }
  if(opcode==2 && type==6){
    double sub = op1-op2;
    if(sub+op2 != op1 || op1-sub != op2){
      ret = true;
    }
    else ret = false;
  }
  if(opcode==3 && type==7){
    double mul = op1*op2;
    if((op2!=0 && mul/op2 != op1) || (op1!=0 && mul/op1 != op2)){
      return true;
    }
    else ret = false;
  }

  return ret;
}

bool GMPEvaluateFInvalid(double op1, int type){

  if (std::isinf(op1) || std::isnan(op1))
    return false;

  if (type == 1 && op1 < 0)
    return true;
  if (type == 2 && op1 < 0)
    return true;
/* if (type == 3 && op1 == 0)//base == 0
    return true;
*/
  return false;
}

bool GMPEvaluateFDiv(double op1,double op2, int type){
  // GMP lib can not accept nan/inf
  if (std::isinf(op1) || std::isnan(op1) ||
      std::isinf(op2) || std::isnan(op2))
    return false;

  if (type == 3){
    if (op1 == 0 && op2 == 0)
      return true;
    return false;
  }

  if (type == 4){
    if (op1 != 0 && op2 == 0)
      return true;
    return false;
  }
  bool ret = false;

  double absOp1 = fabs(op1);
  double absOp2 = fabs(op2);
/*
  long int_value1 = 0x7fefffffffffffff;
  long int_value2 = 0x0000000000000001;
  double *dmax = (double *)&int_value1;
  double *dmin = (double *)&int_value2;
*/
  mpq_t hVal1, hVal2, DMax, DMin, DZero, hResMax, hResMin;
  mpq_init(hVal1);mpq_init(hVal2);
  mpq_init(DMax);mpq_init(DMin);mpq_init(DZero);
  mpq_init(hResMax);mpq_init(hResMin);
  mpq_set_d(hVal1,absOp1);
  mpq_set_d(hVal2,absOp2);
  mpq_set_d(DMax,DBL_MAX);
  mpq_set_d(DMin,DBL_MIN);
  //mpq_set_d(DMax,*dmax);
  //mpq_set_d(DMin,*dmin);
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



extern "C" {

bool FPCHECK_FADD_OVERFLOW(double left,double right){
  return GMPEvaluateComm(left, right, 1, 1);
}
bool FPCHECK_FADD_UNDERFLOW(double left,double right){
  return GMPEvaluateComm(left, right, 1, 2);
}
bool FPCHECK_FSUB_OVERFLOW(double left,double right){
  return GMPEvaluateComm(left, right, 2, 1);
}
bool FPCHECK_FSUB_UNDERFLOW(double left,double right){
  return GMPEvaluateComm(left, right, 2, 2);
}
bool FPCHECK_FMUL_OVERFLOW(double left,double right){
  return GMPEvaluateComm(left, right, 3, 1);
}
bool FPCHECK_FMUL_UNDERFLOW(double left,double right){
  return GMPEvaluateComm(left, right, 3, 2);
}
bool FPCHECK_FDIV_OVERFLOW(double left,double right){
  return GMPEvaluateFDiv(left, right, 1);
}
bool FPCHECK_FDIV_UNDERFLOW(double left,double right){
  return GMPEvaluateFDiv(left, right, 2);
}
bool FPCHECK_FDIV_INVALID(double left,double right){
  return GMPEvaluateFDiv(left, right, 3);
}
bool FPCHECK_FDIV_ZERO(double left,double right){
  return GMPEvaluateFDiv(left, right, 4);
}

bool FPCHECK_FINVALID_SQRT(double left,double right){
  return GMPEvaluateFInvalid(left, 1);
}
bool FPCHECK_FINVALID_LOG(double left,double right){
  return GMPEvaluateFInvalid(left, 2);
}
bool FPCHECK_FINVALID_POW(double left,double right){
  return GMPEvaluateFInvalid(left, 3);
}

bool FPCHECK_FADD_ACCURACY(double left,double right){
  return GMPEvaluateComm(left, right, 1, 5);
}
bool FPCHECK_FSUB_ACCURACY(double left,double right){
  return GMPEvaluateComm(left, right, 2, 6);
}
bool FPCHECK_FMUL_ACCURACY(double left,double right){
  return GMPEvaluateComm(left, right, 3, 7);
}
bool FPCHECK_FDIV_ACCURACY(double left,double right){
  return GMPEvaluateFDiv(left, right, 8);
}

}
