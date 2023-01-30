import os
import pandas as pd
import re
import openpyxl
import csv
from glob import glob

data_path1 = './result/Obj1Greedy'
data_path2 = './result/INSGA2-54'
save_path = './result/Obj1Greedy_and_INSGA2-54'

namelist1 = sorted(glob(os.path.join(data_path1, 'data_*.csv')), key=lambda f: int(re.sub('\D', '', f)))
namelist2 = sorted(glob(os.path.join(data_path2, 'data_*.csv')), key=lambda f: int(re.sub('\D', '', f)))

for file_name1, file_name2 in zip(namelist1, namelist2):
    instanceNo = file_name1.split('/')[-1][:-4]
    save_file_name = save_path + '/' + instanceNo + '.csv'
    csv_file1 = pd.read_csv(file_name1)
    csv_file2 = pd.read_csv(file_name2)
    merged_file = pd.concat((csv_file1, csv_file2), ignore_index=True)
    merged_file.to_csv(save_file_name, index=False)

print('OK!')
