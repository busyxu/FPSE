import fileinput
import subprocess
import os
import re

def comment_lines_containing_string(file_path, target_string):
    with fileinput.FileInput(file_path, inplace=True, backup='.bak') as file:
        for line in file:
            if target_string in line:
                print("//" + line.rstrip())
            else:
                print(line, end="")

def uncomment_lines_containing_string(file_path, target_string):
    with fileinput.FileInput(file_path, inplace=True, backup='.bak') as file:
        for line in file:
            if target_string in line:
                uncommented_line = re.sub(r'(?<!\\)//', '', line)
                print(uncommented_line, end="")
            else:
                print(line, end="")

# Set the directory path where your C programs are located
c_programs_dir = '/home/aaa/gsl/drives/bench_blas/blas'

# Get a list of all the C program files in the directory
c_programs = [file for file in os.listdir(c_programs_dir) if file.endswith('.c')]

# Compile and run each C program
for program in c_programs:

    # 指定 C 程序文件路径
    c_program_path = os.path.join(c_programs_dir, program)
    print(c_program_path)

    # 指定要包含的字符串
    target_string = 'klee'

    # 调用函数注释包含指定字符串的行
    uncomment_lines_containing_string(c_program_path, target_string)