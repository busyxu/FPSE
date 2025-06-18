import json
import sys
import os
import math

# note : usage is : python3 'dirver_name' 'getFile/getCov'
argvs = sys.argv[1:]
driver_name = argvs[0] #such as "gsl_sf_mathieu_b_e"
opt = argvs[1]
all_coverage_path = argvs[2]

#print('  ',opt,' ', driver_name)
gcov_path = opt
# gcov_dir = os.path.split(gcov_path)[0]
# all_coverage_path = gcov_dir + "/temp_branch.out"

branch_count = 0
with open(str(all_coverage_path),'r') as f:
  lines = f.readlines()
  for line in lines:
    #print(line, ' === ')
    line = line.replace('\n','')
    if "Taken at least once:" in line:
      data = line.split(':')[1].split("of")
      precent = float(data[0].split('%')[0])
      total = int(data[1])
      branch_count += int(total * precent / 100)
print(branch_count)      



