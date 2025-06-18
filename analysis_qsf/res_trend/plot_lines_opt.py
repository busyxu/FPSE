import os
import matplotlib.pyplot as plt
import numpy as np
from scipy.interpolate import interp1d
import math
from scipy.stats import norm


def confidence_interval(mean, std_dev, confidence=0.95, n=1):
    z_value = norm.ppf((1 + confidence) / 2)
    margin_of_error = z_value * std_dev / math.sqrt(n)
    # doubleStd = 2*std_dev
    # tripleStd = 3*std_dev
    # return f'{mean:.2f} ({margin_of_error:.2f})'
    # lower_bound = mean - margin_of_error
    # upper_bound = mean + margin_of_error
    # return f'({lower_bound:.2f}, {upper_bound:.2f})'

    # return f'{mean:.2f} ({doubleStd:.2f})'
    # lower_bound = mean - doubleStd
    # upper_bound = mean + doubleStd
    # return f'({lower_bound:.2f}, {upper_bound:.2f})'

    # return f'{mean:.2f} ({tripleStd:.2f})'
    # lower_bound = mean - tripleStd
    # upper_bound = mean + tripleStd
    # return f'({lower_bound:.2f}, {upper_bound:.2f})'
    return margin_of_error


def drawPlot(csv_files, filename):
    Y_list = []
    X = [i for i in range(32)[::2]]
    for file in csv_files:
        xSample = []
        ySample = []
        Y = []
        with open("csv_opt/" + file, 'r') as f:
            i = 0
            for line in f:
                strLine = line.strip('\n').split(',')
                strToInt = [float(i) for i in strLine]
                if i % 2 == 0:
                    xSample.append(strToInt)
                else:
                    ySample.append(strToInt)
                    f = interp1d(xSample[-1], ySample[-1], kind='linear')
                    yInterp = f(X)
                    Y.append(yInterp)
                i += 1

        # X_list.append(X)
        Y_list.append(Y)

    y_mean = np.mean(Y_list, axis=0)
    y_std = np.std(Y_list, axis=0)
    y_moe = confidence_interval(y_mean, y_std, 0.99, 20)
    # print(y_std)
    # marker = ['.', 'x', '+', '.', 'x', '.', '.', '.', '.']
    marker = ['^', 'd', 's', 'p', 'h', '*', 'x', '.']
    label = ['Z3', 'Bitwuzla', 'MathSAT5', 'CVC5', 'COLIBRI', 'JFS', 'goSAT', 'QSF']  # 标签序列参数
    # color = ['b', 'b', 'b', 'y', 'y', 'r', 'c', 'm', 'g']  # 颜色参数序列
    # alpha = [0.2, 0.23, 0.26, 0.29, 0.32, 0.35, 0.38, 0.41, 0.44]

    fig, ax = plt.subplots()
    for i in range(len(y_mean)):
        ax.plot(np.array(X), np.array(y_mean[i]),
                marker=marker[i],
                markersize=None,
                markerfacecolor=None,
                markeredgecolor=None,
                alpha=0.5,
                ls=None,
                label=label[i],
                linewidth=2)
        ax.fill_between(X, y_mean[i] - y_moe[i], y_mean[i] + y_moe[i], alpha=0.5)

    ax.tick_params(right=True, top=True)
    ax.spines['bottom'].set_linewidth(0.1)
    ax.spines['left'].set_linewidth(0.1)
    ax.spines['right'].set_linewidth(0.1)
    ax.spines['top'].set_linewidth(0.1)
    # 参数设置
    ax.tick_params(direction='in', labelsize=10)
    # 坐标轴标签设置
    ax.set_xlabel("Elapsed Timed (min)", fontsize=12)
    ax.set_ylabel("Total Number of Covered Statements", fontsize=12)

    # 图例设置
    # ax.legend(fontsize=12, edgecolor='black', loc='lower right', ncol=3)
    ax.legend(bbox_to_anchor=(0.5, 1.15), loc='upper center', ncol=4)
    # plt.show()

    plt.savefig(filename, bbox_inches='tight', pad_inches=0.01)
    # plt.savefig("figyx/" + cname + "_bfs_covtrend.pdf", dpi=300, bbox_inches='tight', pad_inches=0.1)
    plt.close()


def getDataFromCSV(searchtype, item):

    # 文件夹路径
    folder_path = 'csv_opt'

    # 获取文件夹中所有文件的列表
    files = os.listdir(folder_path)

    # 过滤出包含特定关键词的 .csv 文件
    csv_files = [file for file in files if file.endswith('.csv') and searchtype in os.path.basename(file)
                 and item in os.path.basename(file)]

    # 输出结果
    print("筛选后的 CSV 文件列表:", csv_files)
    return csv_files

csv_files_bfs = getDataFromCSV('bfs', 'lines')
csv_files_dfs = getDataFromCSV('dfs', 'lines')
# csv_files_bfs_algorithm = getDataFromCSV('algorithm', 'bfs', 'lines')
# csv_files_dfs_algorithm = getDataFromCSV('algorithm', 'dfs', 'lines')
# csv_files_bfs_sf = getDataFromCSV('sf', 'bfs', 'lines')
# csv_files_dfs_sf = getDataFromCSV('sf', 'dfs', 'lines')
#
drawPlot(csv_files_bfs, 'trend_pdf_opt/trend_lines_bfs.pdf')
drawPlot(csv_files_dfs, 'trend_pdf_opt/trend_lines_dfs.pdf')
# drawPlot(csv_files_bfs_algorithm, 'trend_pdf_opt/algorithm_bfs_trend_lines.pdf')
# drawPlot(csv_files_dfs_algorithm, 'trend_pdf_opt/algorithm_dfs_trend_lines.pdf')
# drawPlot(csv_files_bfs_sf, 'trend_pdf_opt/sf_bfs_trend_lines.pdf')
# drawPlot(csv_files_dfs_sf, 'trend_pdf_opt/sf_dfs_trend_lines.pdf')

