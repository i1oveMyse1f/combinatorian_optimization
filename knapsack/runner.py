import os, sys
import pandas as pd
import re

_, solver, exp_name = sys.argv

if exp_name not in os.listdir('./answers'):
    os.mkdir(f'./answers/{exp_name}')
results = pd.read_csv('./answers/results.csv', sep=';').set_index('algo')
cur_res = {}

for i in range(1, 11):
    print('Test', i)
    
    os.system(f'./{solver} < ./tests/{i}.public > ./answers/{exp_name}/{i}.out')

    submition_res = os.popen(f'python3 submit.py {i}.public answers/{exp_name}/{i}.out').readlines()
    ok_str = '         Submission score: '
    if (len(submition_res) == 2) and (submition_res[1][0:len(ok_str)] == ok_str):
        score = int(re.sub('\D', '', submition_res[1]))
        #print(submition_res)
        print(f'\tScore = {score}')
        cur_res[f'{i}.public'] = score
    else:
        print('Fail\n', submition_res)
        cur_res[f'{i}.public'] = 'Fail'

results.loc[exp_name] = cur_res
print(results)
results.to_csv('./answers/results.csv', sep=';')
