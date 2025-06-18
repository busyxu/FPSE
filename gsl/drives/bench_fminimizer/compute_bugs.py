import time
import os

cmdStr = "grep -rn \"FloatPointCheck: \""
with os.popen(cmdStr, 'r') as f:
  resAll = f.read()
resList = resAll.split('\n')

resVec = [0,0,0,0,0]
uniqOver = []
uniqUnder = []
uniqInvalid = []
uniqZero = []
for i in range(len(resList)):
  lineStr = resList[i]
  if ".runlog" not in lineStr:
    continue
  if "libm" in lineStr:
    continue
  resVec[0] += 1
  locStr = lineStr.split("KLEE: ERROR: ")[1].split(": FloatPointCheck:")[0]
  if "Overflow found" in lineStr:
    resVec[1] += 1
    if locStr not in uniqOver:
      uniqOver.append(locStr)
  if "Underflow found" in lineStr:
    resVec[2] += 1
    if locStr not in uniqUnder:
      uniqUnder.append(locStr)
  if "Invalid found" in lineStr:
    resVec[3] += 1
    if locStr not in uniqInvalid:
      uniqInvalid.append(locStr)
  if "Zero found" in lineStr:
    resVec[4] += 1
    if locStr not in uniqZero:
      uniqZero.append(locStr)

print(resVec)
print(len(uniqOver),len(uniqUnder),len(uniqInvalid),len(uniqZero))

