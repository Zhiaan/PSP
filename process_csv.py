import os
import pandas as pd
import re
import openpyxl
from glob import glob

data_path = './result/Obj1Greedy'
example_path = './result/example.xlsx'
save_path = './result/作品上传示例.xlsx'

# 打开一个新的 Excel 文件
workbook = openpyxl.load_workbook(example_path)

namelist = sorted(glob(os.path.join(data_path, 'data_*.csv')), key=lambda f: int(re.sub('\D', '', f)))
# namelist = ['./result/INSGA2/data_130.csv']

for file_name in namelist:
    instanceNo = file_name.split('/')[-1][:-4]
    print(instanceNo)
    with open(file_name, 'r', encoding='utf-8') as csvfile:
        sheet = workbook[instanceNo]
        # 读取 csv 文件中的数据
        csv = pd.read_csv(csvfile, dtype=int)
        rows_num = csv.shape[0]
        cols_num = csv.shape[1]

        if rows_num < 50:
            sheet.delete_rows(rows_num + 2, 52)

        for i in range(0, rows_num):
            for j in range(0, cols_num):
                sheet.cell(i + 2, j + 1).value = csv.iloc[i, j]

# 保存 Excel 文件
workbook.save(save_path)
workbook.close()

# for check, compare csv
# ok = pd.read_excel('result_save.xlsx')
# ok.to_csv('result_save.csv', index=None, header=True)

print('OK!')
