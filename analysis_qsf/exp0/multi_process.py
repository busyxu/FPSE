import multiprocessing
import time
import os

# 要删除的文件路径
file_to_delete = "benchmark_bug.txt"

# 检查文件是否存在
if os.path.exists(file_to_delete):
    # 如果文件存在，删除文件
    os.remove(file_to_delete)

# 要删除的文件路径
file_to_delete = "benchmark_new.txt"

# 检查文件是否存在
if os.path.exists(file_to_delete):
    # 如果文件存在，删除文件
    os.remove(file_to_delete)


def runOneTestCase(workDic, testName, solver_type, search_type):
  tempname=workDic+"&"+testName[:-2]+"&"+solver_type+"&"+search_type+"_output"
  filename = os.path.join(workDic, tempname, "test000001.ktest")

  # if os.path.isfile(filename):
  #   print(tempname, "exit")
  # else:
  #   #print("=============")
  #   cmdStr = "./run_solver.sh %s %s %s %s" %(workDic, testName, solver_type, search_type)
  #   print(cmdStr)
  #   with open("benchmark_bug.txt", 'a') as f:
  #     f.write(filename + "\n")
  #   os.system(cmdStr)

  cmdStr = "./run_solver.sh %s %s %s %s" % (workDic, testName, solver_type, search_type)
  print(cmdStr)
  with open("benchmark_bug.txt", 'a') as f:
    f.write(filename + "\n")
  os.system(cmdStr)

directory = '.'
file_pattern = '*.c'

with os.popen(r'find {} -type f -name "{}"'.format(directory, file_pattern), 'r') as f:
  testcase = f.read()
testList = testcase.split('\n')
for test in testList:
    print(test)

cnt = 0
pool = multiprocessing.Pool(processes=60)
for i in range(len(testList[:-1])):
  if len(testList[i]) < 7:
    break
  #if i > 3: break
  testStr = testList[i][2:].split('/')
  workDic = testStr[0]
  testName = testStr[1]
  # if workDic != "elementary" and workDic != "cdf" and workDic != "complex":
  #   continue
  # print(workDic)
  # if workDic == "sf":
  #   continue

  solver_type = ["z3", "bitwuzla", "mathsat5", "cvc5", "colibri", "jfs", "gosat", "qsf"]
  # solver_type = ["z3"]
  # print(workDic, testName)
  for solver in solver_type:
    pool.apply_async(runOneTestCase, (workDic, testName, solver, "bfs"))
    cnt += 1
    pool.apply_async(runOneTestCase, (workDic, testName, solver, "dfs"))
    cnt += 1
    print("======", cnt, "======")
pool.close()
pool.join()

print("======>total:", cnt)
