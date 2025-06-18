import csv

import pandas as pd

from benchmark.multi_process import solver

max_columns = 0
rows = []

with open('/home/aaa/fp-solver/analysis_opt/res_trend/csv_opt/trend_branches_bfs_0.csv', 'r') as f:
    reader = csv.reader(f)
    for row in reader:
        max_columns = max(max_columns, len(row))
        rows.append(row)

for i, row in enumerate(rows):
    while len(row) < max_columns:
        row.append(None) # row size inc 1

df = pd.DataFrame(rows)

# df = pd.read_csv('/home/aaa/fp-solver/analysis_opt/res_trend/csv_opt/trend_branches_bfs_0.csv')

# 假设每两行代表一个求解器的数据
solver_4_data = df.iloc[6:8].reset_index(drop=True)  # 第4个求解器的数据（第7行和第8行）
solver_8_data = df.iloc[14:16].reset_index(drop=True)  # 第8个求解器的数据（第15行和第16行）

# 合并数据：选择每一列时间更短的
# merged_data = solver_4_data.copy()
dict = {
    'x' : [],
    'y' : []
}

[n1,m1] = solver_4_data.shape
[n2,m2] = solver_8_data.shape

i=0
j=0

while i<m1 and j<m2:
    if solver_4_data.iloc[0,i] < solver_8_data.iloc[0,j]:

for i in range(m):
    score1 = float(solver_4_data.loc[1,i])/float(solver_4_data.loc[0,i])
    score2 = float(solver_8_data.loc[1, i]) / float(solver_8_data.loc[0, i])

    if solver_8_data.loc[i, '时间'] < solver_4_data.loc[i, '时间']:
        merged_data.loc[i, '时间'] = solver_8_data.loc[i, '时间']
        merged_data.loc[i, '覆盖率'] = solver_8_data.loc[i, '覆盖率']
    else:
        merged_data.loc[i, '覆盖率'] = solver_4_data.loc[i, '覆盖率']

# 将合并后的数据保存到新的CSV文件
merged_data.to_csv('csv_opt/trend_branches_bfs_0_merged.csv', index=False)

print("合并完成，已保存为 'merged_output.csv'")
