import multiprocessing
import time
import os

def runOneTestCase(workDic, testName, solver_type, search_type):
  tempname=testName[:-2]+"&"+solver_type+"&"+search_type+"_output"
  filename = os.path.join(workDic, tempname, "execute_time.txt")
  #print(filename)

  if os.path.isfile(filename):
    folder_path = os.path.join(workDic, tempname)
    file_list = os.listdir(folder_path)
    file_count = len(file_list)
    #print(folder_path,file_count)  
    if file_count>=6:
      print(tempname,"exit")
    else:
      #print("=============")
      cmdStr = "./run_solver.sh %s %s %s %s" %(workDic, testName, solver_type, search_type)
      print(cmdStr)
      os.system(cmdStr)
  else:
    #print("=============")
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
  pool.apply_async(runOneTestCase, (workDic,testName,"fp2int","bfs"))
  pool.apply_async(runOneTestCase, (workDic,testName,"jfs","bfs"))
  pool.apply_async(runOneTestCase, (workDic,testName,"gosat","bfs"))
  pool.apply_async(runOneTestCase, (workDic,testName,"smt-dreal","bfs"))

  pool.apply_async(runOneTestCase, (workDic,testName,"smt","dfs"))
  pool.apply_async(runOneTestCase, (workDic,testName,"bitwuzla","dfs"))
  pool.apply_async(runOneTestCase, (workDic,testName,"mathsat5","dfs"))
  pool.apply_async(runOneTestCase, (workDic,testName,"cvc5-real","dfs"))
  pool.apply_async(runOneTestCase, (workDic,testName,"dreal-is","dfs"))
  pool.apply_async(runOneTestCase, (workDic,testName,"fp2int","dfs"))
  pool.apply_async(runOneTestCase, (workDic,testName,"jfs","dfs"))
  pool.apply_async(runOneTestCase, (workDic,testName,"gosat","dfs"))
  pool.apply_async(runOneTestCase, (workDic,testName,"smt-dreal","dfs"))
  
pool.close()
pool.join()
