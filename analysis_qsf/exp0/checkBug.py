import os
import shutil

def recreate_folder(folder_path):
    if os.path.exists(folder_path):
        # 删除现有文件夹及其内容
        shutil.rmtree(folder_path)

    # 创建新文件夹
    os.makedirs(folder_path)


def search_and_copy_runlog_files(source_folder, target_folders, target_strings):
    # 初始化计数器字典，用于跟踪每个目标文件夹中的文件数量
    file_count = {target_folder: 0 for target_folder in target_folders}

    # 遍历源文件夹中的所有文件和子文件夹
    for root, dirs, files in os.walk(source_folder):
        if root != "./instances":
            continue
        for file in files:
            if file.endswith('.runlog'):
                file_path = os.path.join(root, file)
                with open(file_path, 'r') as f:
                    content = f.read()
                    flag = False
                    for target_folder, target_string in zip(target_folders, target_strings):
                        if target_string in content:
                            # 构造目标文件路径
                            target_file_path = os.path.join(target_folder, file)
                            # 复制文件到目标文件夹
                            shutil.copy2(file_path, target_file_path)
                            print(f"Copied file '{file}' to '{target_folder}'")
                            # 增加目标文件夹的文件数量计数
                            file_count[target_folder] += 1
                            flag = True
                            break
                    if flag==False:
                        target_folder = target_folders[-1]
                        # 构造目标文件路径
                        target_file_path = os.path.join(target_folder, file)
                        # 复制文件到目标文件夹
                        shutil.copy2(file_path, target_file_path)
                        print(f"Copied file '{file}' to '{target_folder}'")
                        # 增加目标文件夹的文件数量计数
                        file_count[target_folder] += 1

    # 打印每个目标文件夹的文件数量
    for target_folder, count in file_count.items():
        print(f"Found {count} files in '{target_folder}'")


# # 目标文件夹和目标字符串列表
# target_folders = ['../dump_runlog', '../good_runlog', '../timeout_runlog', '../bug_runlog', '../unknown_runlog']
# target_strings = [
#     'KLEE: halting execution, dumping remaining states',
#     'KLEE: done: generated tests = ',
#     'WARNING: KLEE: WATCHDOG: kill(9)ing child (I tried to be nice)',
#     'KLEE: WARNING: KLEE: watchdog exiting (no child)'
# ]

# 目标文件夹和目标字符串列表
target_folders = ['runlog/good_runlog', 'runlog/timeout_runlog', 'runlog/main_runlog', 'runlog/alloc_runlog', 'runlog/bug_runlog', 'runlog/unknown_runlog']
target_strings = [
    'KLEE: done: generated tests = ',
    'WARNING: KLEE: WATCHDOG: kill(9)ing child (I tried to be nice)',
    'KLEE: ERROR: Could not link KLEE files Entry function',
    'tcmalloc: large alloc',
    'KLEE: WARNING: KLEE: watchdog exiting (no child)'
]

for target_folder in target_folders:
    # 删除并重建文件夹
    recreate_folder(target_folder)

# 调用函数并传入源文件夹路径、目标文件夹列表和目标字符串列表
search_and_copy_runlog_files('./', target_folders, target_strings)
