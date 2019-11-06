import numpy as np

M = dict()
np.random.seed(3)

# 第一种数据划分方式
def my_train_test_split(data, test_size=0.2, random_state=3):
    train_set = dict()
    test_set = dict()
    np.random.seed(random_state)

    for item in data.items():
        if np.random.rand() < test_size:
            test_set[item[0]] = item[1]
        else:
            train_set[item[0]] = item[1]
    return train_set, test_set

row_number = 0
col_number = 0

with open('./out.txt', 'r') as f:
    for line in f.readlines():
        line = line.replace('\n', '')
        i, j, v = line.split(',')
        i = int(i)
        j = int(j)
        v = int(v)
        row_number = max(i, row_number)
        col_number = max(j, col_number)
        if i in M:
            M[i].append((j, v))
        else:
            M[i] = [(j, v)]
row_number += 1

# 第二种数据划分方式
with open('./train-split-user.txt', 'w') as tf:
    with open('./test-split-user.txt', 'w') as testF:
        for item in M.items():
            i = item[0]
            arr = np.array(item[1])
            length = arr.shape[0]
            index_1 = np.random.choice(length, int(0.2 * length), replace=False)
            index_2 = np.delete(np.arange(length), index_1)
            for j, v in arr[index_1]:
                testF.write(str(i) + "," + str(j) + "," + str(v) + "\n")
            for j, v in arr[index_2]:
                tf.write(str(i) + "," + str(j) + "," + str(v) + "\n")
