import fileinput
import subprocess
import os
import re

def delete_backup_files(directory):
    for filename in os.listdir(directory):
        if filename.endswith('.bak'):
            file_path = os.path.join(directory, filename)
            os.remove(file_path)

# 指定备份文件所在的目录
backup_directory = '/home/aaa/gsl/drives/bench_complexNum/complexNum'

# 调用函数删除备份文件
delete_backup_files(backup_directory)

# def comment_lines_containing_string(file_path, target_string):
#     with fileinput.FileInput(file_path, inplace=True, backup='.bak') as file:
#         for line in file:
#             if target_string in line:
#                 print("//" + line.rstrip())
#             else:
#                 print(line, end="")
#
# def uncomment_lines_containing_string(file_path, target_string):
#     inside_comment = False
#
#     with fileinput.FileInput(file_path, inplace=True, backup='.bak') as file:
#         for line in file:
#             if not inside_comment and target_string in line:
#                 line = re.sub(r'(?<!\\)//', '', line)
#                 line = re.sub(r'/\*.*?\*/', '', line)
#                 print(line, end="")
#             elif '/*' in line:
#                 inside_comment = True
#                 line = re.sub(r'/\*.*', '', line)
#                 print(line, end="")
#             elif '*/' in line:
#                 inside_comment = False
#                 line = re.sub(r'.*?\*/', '', line)
#                 print(line, end="")
#             else:
#                 print(line, end="")
#
# # Set the directory path where your C programs are located
# c_programs_dir = '/home/aaa/gsl/drives/bench_complexNum/complexNum'
#
# # Get a list of all the C program files in the directory
# c_programs = [file for file in os.listdir(c_programs_dir) if file.endswith('.c')]
#
# # Compile and run each C program
# for program in c_programs:
#
#     # 指定 C 程序文件路径
#     c_program_path = os.path.join(c_programs_dir, program)
#     print(c_program_path)
#
#     # 指定要包含的字符串
#     target_string = 'klee'
#
#     # # 调用函数注释包含指定字符串的行
#     # comment_lines_containing_string(c_program_path, target_string)
#
#     # 调用函数解除包含指定字符串的行的注释
#     uncomment_lines_containing_string(c_program_path, target_string)


#
# import os
#
# def add_comment_to_first_line(file_path, comment):
#     with open(file_path, 'r') as file:
#         lines = file.readlines()
#
#     with open(file_path, 'w') as file:
#         file.write("// " + comment + "\n")
#         file.writelines(lines)
#
# # 指定 C 程序文件所在的目录
# c_programs_directory = '/home/aaa/gsl/drives/bench_blas/blas'
#
# # 指定要添加的注释信息
# comment = '======== add by yx ========'
#
# # 遍历目录中的每个 C 程序文件
# for filename in os.listdir(c_programs_directory):
#     if filename.endswith('.c'):
#         file_path = os.path.join(c_programs_directory, filename)
#         add_comment_to_first_line(file_path, comment)