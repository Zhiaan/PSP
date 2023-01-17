import os
import pandas as pd
import re
import csv
import openpyxl
from openpyxl.styles import Font
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

        carNum = 0
        # 将数据写入 sheet1
        for row in reader:
            sheet.append(row)
            carNum = len(row) - 3
        for j in range(1, carNum + 4):
            cell = sheet.cell(row=1, column=j)
            cell.font = Font(name="等线 (正文)", size=11)
        for i in range(2, 52):
            for j in range(1, carNum + 4):
                cell = sheet.cell(row=i, column=j)
                cell.data_type = "int"
                cell.font = Font(name="等线 (正文)", size=11)

del workbook["Sheet"]
# 保存 Excel 文件
workbook.save('./result/result.xlsx')
print('OK!')