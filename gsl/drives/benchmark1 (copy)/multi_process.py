import multiprocessing
import time
import os

def runOneTestCase(workDic, testName, solver_type, search_type):
  #filename = workDic+"/"+testName[:-2]+"_output"+"/execute_time.txt"
  tempname=testName[:-2]+"&"+solver_type+"&"+search_type+"_output"
  filename = os.path.join(workDic, tempname, "execute_time.txt")
  #print(filename)
  if os.path.isfile(filename):
    print(tempname,"exit");
  else:
    cmdStr = "./run_solver.sh %s %s %s %s" %(workDic, testName, solver_type, search_type)
    print(cmdStr)
    os.system(cmdStr)

directory = '.'
file_pattern = '*.c'

with os.popen(r'find {} -type f -name "{}"'.format(directory, file_pattern), 'r') as f:
  testcase = f.read()
testList = testcase.split('\n')
print(testList[:-1])

pool = multiprocessing.Pool(processes=12)
for i in range(len(testList[:-1])):
  if len(testList[i]) < 7:
    break
  #if i > 3: break
  testStr = testList[i][2:].split('/')
  workDic = testStr[0]
  testName = testStr[1]

  #print(workDic, testName)
  pool.apply_async(runOneTestCase, (workDic,testName,"smt","bfs"))
  pool.apply_async(runOneTestCase, (workDic,testName,"bitwuzla","bfs"))
  pool.apply_async(runOneTestCase, (workDic,testName,"mathsat5","bfs"))
  pool.apply_async(runOneTestCase, (workDic,testName,"cvc5-real","bfs"))
  pool.apply_async(runOneTestCase, (workDic,testName,"dreal-is","bfs"))
  pool.apply_async(runOneTestCase, (workDic,testName,"jfs","bfs"))
  pool.apply_async(runOneTestCase, (workDic,testName,"gosat","bfs"))
  pool.apply_async(runOneTestCase, (workDic,testName,"smt-dreal","bfs"))

  pool.apply_async(runOneTestCase, (workDic,testName,"smt","dfs"))
  pool.apply_async(runOneTestCase, (workDic,testName,"bitwuzla","dfs"))
  pool.apply_async(runOneTestCase, (workDic,testName,"mathsat5","dfs"))
  pool.apply_async(runOneTestCase, (workDic,testName,"cvc5-real","dfs"))
  pool.apply_async(runOneTestCase, (workDic,testName,"dreal-is","dfs"))
  pool.apply_async(runOneTestCase, (workDic,testName,"jfs","dfs"))
  pool.apply_async(runOneTestCase, (workDic,testName,"gosat","dfs"))
  pool.apply_async(runOneTestCase, (workDic,testName,"smt-dreal","dfs"))
  
pool.close()
pool.join()
