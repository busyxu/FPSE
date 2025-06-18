import os
import matplotlib.pyplot as plt
import numpy as np
from scipy.interpolate import interp1d
from matplotlib.ticker import LogLocator

def drawPlot(csv_files, filename):
    Y_list = []
    X = [i for i in range(62)[::2]]
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
    # print(y_std)
    # marker = ['.', 'x', '+', '.', 'x', '.', '.', '.', '.']
    # marker = ['^', 'd', 's', 'p', 'h', '*', 'x', '.']
    label = ['Z3', 'CVC5', 'MathSAT5', 'Bitwuzla', 'COLIBRI', 'JFS', 'goSAT', 'QSF']  # 标签序列参数
    # solver_type = ["smt", "bitwuzla", "mathsat5", "cvc5", "dreal-is", "jfs", "gosat-is", "optsat", "gosat"]
    # color = ['b', 'y', 'r', 'c', 'm', 'g']  # 颜色参数序列
    # colors = ['#1f77b4', '#ff7f0e', '#2ca02c', '#d62728', '#9467bd', '#8c564b', '#e377c2', '#7f7f7f']
    # alpha = [0.2, 0.23, 0.26, 0.29, 0.32, 0.35, 0.38, 0.41, 0.44]

    fig, ax = plt.subplots()
    # ax.set_yscale('symlog', linthreshy=0.1, linscaley=1)
    ax.set_xscale('symlog', linthreshx=1, linscalex=0.1)
    AxisLocator = LogLocator(base=10, subs=np.arange(1.0, 10.0))

    # ax.yaxis.set_minor_locator(AxisLocator)
    # ax.yaxis.set_tick_params(which='minor', length=4, labelsize=pargs.tick_font_size)
    # ax.yaxis.set_tick_params(which='major', length=6, labelsize=pargs.tick_font_size)
    # assert pargs.max_exec_time > 0.0
    # ax.set_ybound(lower=0.0, upper=pargs.max_exec_time)

    ax.xaxis.set_minor_locator(AxisLocator)
    ax.xaxis.set_tick_params(direction='in', which='minor', length=4, labelsize=10)
    ax.xaxis.set_tick_params(direction='in', which='major', length=6, labelsize=10)
    # assert pargs.max_exec_time > 0.0
    ax.set_xbound(lower=0.0, upper=70)

    for i in range(len(y_mean)):
        ax.plot(np.array(X), np.array(y_mean[i]),
                # marker=marker[i],
                # color=colors[i],
                markersize=None,
                markerfacecolor=None,
                markeredgecolor=None,
                alpha=0.5,
                ls=None,
                label=label[i],
                linewidth=2)
        ax.fill_between(X, y_mean[i] - y_std[i], y_mean[i] + y_std[i], alpha=0.5)

    ax.tick_params(right=True, top=True)
    ax.spines['bottom'].set_linewidth(0.1)
    ax.spines['left'].set_linewidth(0.1)
    ax.spines['right'].set_linewidth(0.1)
    ax.spines['top'].set_linewidth(0.1)
    # 参数设置
    ax.tick_params(direction='in', labelsize=10)
    # 坐标轴标签设置
    ax.set_xlabel("Elapsed Timed (min)", fontsize=12)
    ax.set_ylabel("Total Number of Covered Branches", fontsize=12)

    # 图例设置
    # ax.legend(fontsize=12, edgecolor='black', loc='lower right', ncol=3)
    ax.legend(bbox_to_anchor=(0.5, 1.15), loc='upper center', ncol=4)
    # plt.show()

    plt.rcParams['pdf.fonttype'] = 42  # TrueType 字体
    plt.rcParams['ps.fonttype'] = 42
    # plt.savefig(filename, dpi=300, bbox_inches='tight', pad_inches=0.01)
    plt.savefig(filename, bbox_inches='tight', pad_inches=0.01)
    # plt.savefig("figyx/" + cname + "_bfs_covtrend.pdf", dpi=300, bbox_inches='tight', pad_inches=0.1)
    plt.close()


def getDataFromCSV(searchtype, item):

    # 文件夹路径
    folder_path = 'csv_opt'

    # 获取文件夹中所有文件的列表
    files = os.listdir(folder_path)

    # 过滤出包含特定关键词的 .csv 文件
    csv_files = [file for file in files if
                 file.endswith('.csv') and searchtype in os.path.basename(
                     file) and item in os.path.basename(file)]

    # 输出结果
    print("筛选后的 CSV 文件列表:", csv_files)
    return csv_files


csv_files_bfs = getDataFromCSV('bfs_60', 'branches')
csv_files_dfs = getDataFromCSV('dfs_60', 'branches')

drawPlot(csv_files_bfs, 'trend_pdf_opt/trend_branches_bfs_60.pdf')
drawPlot(csv_files_dfs, 'trend_pdf_opt/trend_branches_dfs_60.pdf')

