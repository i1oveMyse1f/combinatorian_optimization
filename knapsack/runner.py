import os, sys

_, solver = sys.argv

for i in range(1, 11):
    print('Test', i)
    os.system(f'{solver} < ./tests/{i}.public > ./answers/ibarra/{i}.out')
    os.system(f'python3 submit.py {i}.public answers/ibarra/{i}.out')
