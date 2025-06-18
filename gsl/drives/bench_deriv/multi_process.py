import multiprocessing
import time
import os

def runOneTestCase(workDic, testName):
  cmdStr = "./run_solver.sh %s %s" %(workDic, testName)
  print(cmdStr)
  os.system(cmdStr)

with os.popen(r'find . -name *.c', 'r') as f:
  testcase = f.read()
testList = testcase.split('\n')

pool = multiprocessing.Pool(processes=4)
for i in range(len(testList)):
  if len(testList[i]) < 5:
    break
  #if i > 3: break
  testStr = testList[i][2:].split('/')
  workDic = testStr[0]
  testName = testStr[1]
  pool.apply_async(runOneTestCase, (workDic,testName))
  
pool.close()
pool.join()
