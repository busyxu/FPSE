import json
import sys
import os
import math

# note : usage is : python3 'dirver_name' 'getFile/getCov'
argvs = sys.argv[1:]
driver_name = argvs[0] #such as "gsl_sf_mathieu_b_e"
opt = argvs[1]
gcov_res = argvs[2]

# driver_name = "gsl_fit_wlinear"
# opt = "/home/aaa/fp-solver/analysis/benchmark3/compAndopt/linear.c.gcov"
# gcov_res = "======= File 'linear.c' Lines executed:29.55% of 132 linear.c:creating 'linear.c.gcov'"

absolute_path = os.path.split(os.path.realpath(__file__))[0]
f=open(absolute_path + "/gsl_functions_info.json", 'r')
all_info_data=json.load(f)

func_info_Data = all_info_data["FunctionInfo"]

for func in func_info_Data:
  #print(func["FunctionName"])
  if func["FunctionName"] == driver_name:
    begin_line = func["BeginLine"]
    end_line = func["EndLine"]
    file_path = func["FileName"] # real file path
    
    split_loc = file_path.rfind("/") + 1
    if opt == "getFile":
      print(file_path[split_loc:])
      break

    # gcov_path = file_path[:split_loc] + ".libs/" + file_path[split_loc:] + ".gcov"
    gcov_path = opt
    gcov_dir = os.path.split(gcov_path)[0]

    # all_coverage_path = gcov_dir + "/temp.out"
    all_line_count = 0
    #
    # with open(str(all_coverage_path),'r') as f:
    #   line = f.readline()
    #   tokens = line.split(' ')
    tokens = gcov_res.split(' ')
    for idx in range(len(tokens)):
      if tokens[idx].find("executed:") != -1:
        precent = float(tokens[idx].split(':')[1].split('%')[0]) / 100.0
        all_line = int(tokens[idx + 2])  # 程序总行数
        all_line_count += math.ceil(precent*all_line)  # 程序实际覆盖的行数

    # 这里拿到的是外部函数的覆盖信息，下面是整个函数的覆盖信息，比如gsl_blas_drotg里面还调用了cblas_drotg

    with open(str(gcov_path),'r') as f:
      lines = f.readlines()
      analysis_flag = False
      covered_line = 0
      target_line = 0

      for line in lines:
        if line.find(str(begin_line) + ":") != -1:  #分析开始行
          analysis_flag = True
        if line.find(str(end_line) + ":") != -1:  #分析结束行
          analysis_flag = False
          break
        if analysis_flag:
          if line.find("#####") == -1 and line.find("-:") == -1:  # 有数字的行，被分析到了的行
            target_line += 1
            covered_line += 1  # find executed lines begin with number: "6:"
          if line.find("#####") != -1 : #无数字的行，未被分析到的行
            target_line += 1   # compute not coverd line in total
 
      covered_rate = covered_line / target_line * 100 ##被分析到达的有效行/总行数
      print(covered_rate,",",all_line_count)  # 覆盖率和覆盖的总行数
    
    break

