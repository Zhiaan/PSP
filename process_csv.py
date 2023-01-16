import os
import pandas as pd
import re
import csv
import openpyxl
from glob import glob
path = './result/INSGA2'

# 打开一个新的 Excel 文件
workbook = openpyxl.Workbook()

namelist = sorted(glob(os.path.join(path, 'data_*.csv')), key=lambda f: int(re.sub('\D', '', f)))
# namelist = ['data_2722.csv']

for file_name in namelist:
    instanceNo = file_name.split('/')[-1][:-4]
    print(instanceNo)
    with open(file_name, 'r', encoding='utf-8') as csvfile:
        sheet = workbook.create_sheet(instanceNo)
        # 读取 csv 文件中的数据
        reader = csv.reader(csvfile)
        # 将数据写入 sheet1
        for row in reader:
            sheet.append(row)

# 保存 Excel 文件
workbook.save('./result/result.xlsx')
print('OK!')