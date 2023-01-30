import matplotlib.pyplot as plt
import pandas as pd
from glob import glob
import os
import re

data_path = './result/INSGA2-high'
save_path = data_path + '-image'

if os.path.exists(save_path) == False:
    os.mkdir(save_path)

def norm(data):
    return (data - data.min()) / (data.max() - data.min())

if __name__ == '__main__':
    # obj1 = (data.values[:, 0])
    # obj2 = (data.values[:, 1])
    # obj3 = (data.values[:, 2])
    # # 三维图形
    # ax = plt.figure().add_subplot(projection='3d')
    # ax.scatter(obj1, obj2, obj3) # 画出(xs1,ys1,zs1)的散点图。
    # ax.set_xlabel('obj1 label') # 画出坐标轴
    # ax.set_ylabel('obj2 label')
    # ax.set_zlabel('obj3 label')
    # plt.savefig('3d.png')

    # 二维图形
    namelist = sorted(glob(os.path.join(data_path, 'data_*.csv')), key=lambda f: int(re.sub('\D', '', f)))
    for file_name in namelist:
        instanceNo = file_name.split('/')[-1][:-4]
        print(instanceNo)
        data = pd.read_csv(file_name, usecols=['Objective 1', 'Objective 2', 'Objective 3'])
        obj1 = (data.values[:, 0])
        obj2 = (data.values[:, 1])
        obj3 = (data.values[:, 2])
        ax = plt.figure().add_subplot()
        ax.scatter(obj1, obj2) # 画出(xs1,ys1,zs1)的散点图。
        ax.set_xlabel('obj1 label') # 画出坐标轴
        ax.set_ylabel('obj2 label')
        plt.savefig(save_path + '/' + instanceNo + '.png')

print('OK!')
