import numpy as np

test_M = list()
M_I = dict()
M_J = dict()
M_test = dict()
test_total = 0
row_number = col_number = 0
total = 0
d = 15
with open('./train-split-user.txt', 'r') as f:
    for line in f.readlines():
        total += 1
        line = line.replace('\n', '')
        i, j, v = line.split(',')
        i = int(i)
        j = int(j)
        v = int(v)
        row_number = max(i, row_number)
        col_number = max(j, col_number)
        if i in M_I:
            M_I[i].append((j, v))
        else:
            M_I[i] = [(j, v)]

        if j in M_J:
            M_J[j].append((i, v))
        else:
            M_J[j] = [(i, v)]

with open('./test-split-user.txt', 'r') as f:
    for line in f.readlines():
        test_total += 1
        line = line.replace('\n', '')
        i, j, v = line.split(',')
        i = int(i)
        j = int(j)
        v = int(v)
        if i in M_test:
            M_test[i].append((j, v))
        else:
            M_test[i] = [(j, v)]


def rmse(M, U, V, all):
    res = 0
    for item in M.items():
        i = item[0]
        for j, v in item[1]:
            p = U[i].dot(V[j])
            res += (v - p) * (v - p)
    return np.sqrt(res / all)

def UV(lamda=0.1, epsilon=1e-3):
    U = np.random.rand(row_number, d)
    V = np.random.rand(col_number, d)

    rmse_last = rmse(M_I, U, V, total)
    time = 0
    print(time, rmse_last, rmse(M_test, U, V, test_total))
    time += 1
    while True:
        for i in range(row_number):
            t1 = np.zeros(d)
            t2 = lamda * np.eye(d)
            if i in M_I:
                for j, v in M_I[i]:
                    t1 = t1 + v * V[j]
                    V_ = np.expand_dims(V[j], axis=1)
                    t2 = t2 + V_.dot(V_.T)
                U[i] = np.linalg.pinv(t2).dot(t1)

        for j in range(col_number):
            t1 = np.zeros(d)
            t2 = lamda * np.eye(d)
            if j in M_J:
                for i, v in M_J[j]:
                    t1 = t1 + v * U[i]
                    U_ = np.expand_dims(U[i], axis=1)
                    t2 = t2 + U_.dot(U_.T)
                V[j] = np.linalg.pinv(t2).dot(t1)
        
        rmse_this = rmse(M_I, U, V, total)
        print(time, rmse_this, rmse(M_test, U, V, test_total))
        time += 1
        if abs(rmse_this - rmse_last) < epsilon:
            break
        else:
            rmse_last = rmse_this
row_number += 1
col_number += 1
UV()