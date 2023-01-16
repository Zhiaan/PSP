import os
import pandas as pd
import re

import csv
path = "/Users/zoupeng/Desktop/xuelang/PSP/result/INSGA2/"


# 打开一个新的 Excel 文件
writer = pd.ExcelWriter('./result/result.xlsx', engine='openpyxl')

namelist = sorted(os.listdir(path))
namelist.sort(key=lambda f: int(re.sub('\D', '', f)))

# namelist = ['data_2722.csv']

for file_name in namelist:
    instanceNo = file_name.split('.', 1)[0]
    print(instanceNo)
    df = pd.read_csv(path + file_name)
    df.to_excel(writer, sheet_name=instanceNo)

# 保存 Excel 文件
writer.save()