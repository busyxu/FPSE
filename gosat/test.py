from z3 import *
'''
x = FP('x', FPSort(11, 53))
y = FP('y', FPSort(11, 53))
absVal = fpFPToFP(RNE(), fpAbs(x+y), FPSort(15, 113))

c1 = BitVecVal(0x43feffffffffffff, 64)
c2 = BitVecVal(0xf000000000000000, 64)
cFP = fpBVToFP(Concat(c1,c2), FPSort(15, 113))

DBL_MAX = 179769313486231570814527423731704356798070567525844996598917476803157260780028538760589558632766878171540458953514382464234321326889464182768467546703537516986049910576551282076245490090389328944075868508455133942304583236903222948165808559332123348274797826204144723168738177180919299881250404026184124858368.000000
z = FP('z', FPSort(11, 53))
extZ = fpFPToFP(RNE(), z, FPSort(15, 113))

s = Solver()

s.add(Not(fpIsInf(x)))
s.add(Not(fpIsInf(y)))
s.add(Not(fpIsNaN(x)))
s.add(Not(fpIsNaN(y)))
#s.add(z == DBL_MAX)
#s.add(absVal > cFP)


dMax1 = BitVecVal(0x43feffffffffffff, 64)
dMax2 = BitVecVal(0xf000000000000000, 64)
dFP = fpBVToFP(Concat(dMax1,dMax2), FPSort(15, 113))

cArray = Array('c', BitVecSort(32), BitVecSort(8))
bArray = Array('b', BitVecSort(32), BitVecSort(8))
c0 = Select(cArray,0)
c1 = Select(cArray,1)
c2 = Select(cArray,2)
c3 = Select(cArray,3)
c4 = Select(cArray,4)
c5 = Select(cArray,5)
c6 = Select(cArray,6)
c7 = Select(cArray,7)
cConcat = Concat(c7,c6,c5,c4,c3,c2,c1,c0)
print(type(cConcat))
c64 = fpBVToFP(cConcat, FPSort(11, 53))
c128 = fpBVToFP(c64, FPSort(15, 113))

b0 = Select(bArray,0)
b1 = Select(bArray,1)
b2 = Select(bArray,2)
b3 = Select(bArray,3)
b4 = Select(bArray,4)
b5 = Select(bArray,5)
b6 = Select(bArray,6)
b7 = Select(bArray,7)
bConcat = Concat(b7,b6,b5,b4,b3,b2,b1,b0)
b64 = fpBVToFP(bConcat, FPSort(11, 53))
b128 = fpBVToFP(b64, FPSort(15, 113))

absVal = fpFPToFP(RNE(), fpAbs(c64+b64), FPSort(15, 113))

s = Solver()

s.add(Not(fpIsInf(c64)))
s.add(Not(fpIsInf(b64)))
s.add(Not(fpIsNaN(c64)))
s.add(Not(fpIsNaN(b64)))
s.add(absVal > dFP)

'''

# s = Solver()
# 
# #x = FP('x', FPSort(11, 53))
# x = Real('x')
# s.add(x*x*x == 27)
# 
# print(s.check())
# print(s.model())


def convertor(f, status="unknown", name="benchmark", logic=""):
    v = (Ast * 0)()
    if isinstance(f, Solver):
        a = f.assertions()
    if len(a) == 0:
      f = BoolVal(True)
    else:
      f = And(*a)
    return Z3_benchmark_to_smtlib_string(f.ctx_ref(), name, logic, status, "", 0, v, f.as_ast())

x = FP('x', FPSort(11, 53))
y = FP('y', FPSort(11, 53))
# c = FP('c', FPSort(11, 53))
# d = FP('d', FPSort(11, 53))
s = Solver()
s.add(Not(y < 0))
s.add(Not(y==0))
s.add(Not(x==0))
s.add(Not(y > 1))
# s.add(x==2*y)
print(convertor(s, logic="BVFP"))
