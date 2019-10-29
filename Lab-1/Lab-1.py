import numpy as np
import pandas as pd

# 建立从user_id到矩阵下标的映射
user_to_index = dict()
# 建立从artist_id到矩阵下标的映射
artist_to_index = dict()
# user和artist的数量
user_number = 0
artist_number = 0

artist_alias = dict()   # wrong_id, correct_id
artist_data = dict()    # artist_id, artist_name
user_artist = dict()    # (user_index, artist_index), times

with open('./artist_correct_format_alias.txt', 'r') as f:
    for line in f.readlines():
        line = line.replace('\n', '')
        wrong, correct = line.split('\t', 2)
        artist_alias[wrong] = correct

with open('./artist_correct_format_data.txt', 'r') as f:
    for line in f.readlines():
        line = line.replace('\n', '')
        artist_id, artist_name = line.split('\t', 2)
        artist_data[artist_id] = artist_name
        # 如果有重复的artist_id出现，去最后一次出现的值
        if artist_id not in artist_to_index:
            artist_to_index[artist_id] = artist_number
            artist_number = artist_number + 1

with open('./user_artist_data.txt', 'r') as f:
    for line in f.readlines():
        line = line.replace('\n', '')
        user_id, artist_id, times = line.split(' ', 3)
        user_index = -1
        artist_index = -1
        if artist_id in artist_alias:
            artist_id = artist_alias.get(artist_id)

        if artist_id not in artist_to_index:
            continue    # 存在部分artist_id在artist_data.txt中不存在的现象
        else:
            artist_index = artist_to_index.get(artist_id)

        if user_id in user_to_index:
            user_index = user_to_index.get(user_id)
        else:
            user_to_index[user_id] = user_number
            user_index = user_number
            user_number = user_number + 1
        # 如果有重复的user_index, artist_index出现，取最后一次有效的值
        user_artist[(user_index, artist_index)] = eval(times)


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


train_user_artist, test_user_artist = my_train_test_split(user_artist)
print(len(train_user_artist), len(test_user_artist))

print('用户数量', user_number)
print('艺术家数量', artist_number)


def rmse(M, U, V):
    n, d = U.shape
    d_1, m = V.shape
    assert d == d_1

    res = 0.0
    #     for i in range(n):
    #         for j in range(m):
    #             t = M.get((i, j), None)
    #             if t is None:
    #                 continue
    #             p = 0.0
    #             for k in range(d):
    #                 p = p + U[i][k] * V[k][j]
    #             res = res + (p - t) * (p - t)
    for item in M.items():
        i, j = item[0]
        # i = user_to_index.get(i, None)
        # j = artist_to_index.get(j, None)
        # if i is None or j is None:
        #     continue
        p = 0.0
        for k in range(d):
            p = p + U[i][k] * V[k][j]
        res = res + (p - item[1]) * (p - item[1])
    return res


def UV_decomposition(M, n, m, d=2, epsilon=1e-3):
    U = np.ones((n, d))
    V = np.ones((d, m))

    time = 0
    rmse_last = rmse(M, U, V)
    while True:
        print(time, rmse_last)
        time += 1
        u_turn = True   # 下一次进行对U中元素的更新
        ui = uj = vi = vj = 0
        u_finished = v_finished = False    # U和V矩阵元素完成标志
        while True:
            if u_finished and v_finished:
                break
            if v_finished or u_turn:
                u_turn = False
                numerator = 0
                denominator = 0
                for j in range(m):
                    p = 0
                    m_rj = M.get((ui, j), None)
                    if m_rj is None:
                        continue
                    for k in range(d):
                        if k != uj:
                            p = p + U[ui][k] * V[k][j]
                    numerator = numerator + V[uj][j] * (m_rj - p)
                    denominator = denominator + V[uj][j] * V[uj][j]
                # last_u = U[ui][uj]
                # if numerator == 0 or denominator == 0:
                #     continue
                U[ui][uj] = U[ui][uj] if numerator == 0 or denominator == 0 else numerator * 1.0 / denominator
                # rmse_this = rmse(M, U, V)
                # if rmse_this < rmse_last:
                #     rmse_last = rmse_this
                # else:
                #     U[ui][uj] = last_u
                uj += 1
                if uj >= d:
                    ui += 1
                    uj = 0
                    if ui >= n:
                        u_finished = True
            elif u_finished or not u_turn:
                u_turn = True
                numerator = 0
                denominator = 0
                for i in range(n):
                    p = 0
                    m_is = M.get((i, vj), None)
                    if m_is is None:
                        continue
                    for k in range(d):
                        if k != vi:
                            p = p + U[i][k] * V[k][vj]
                    numerator = numerator + U[i][vi] * (m_is - p)
                    denominator = denominator + U[i][vi] * U[i][vi]
                # last_v = V[vi][vj]
                # if numerator == 0 or denominator == 0:
                #     continue
                V[vi][vj] = V[vi][vj] if numerator == 0 or denominator == 0 else numerator * 1.0 / denominator
                # rmse_this = rmse(M, U, V)
                # if rmse_this < rmse_last:
                #     rmse_last = rmse_this
                # else:
                #     V[vi][vj] = last_v
                vj += 1
                if vj >= m:
                    vi += 1
                    vj = 0
                    if vi >= d:
                        v_finished = True
        rmse_this = rmse(M, U, V)
        if abs(rmse_last - rmse_this) < epsilon:
            break
        else:
            rmse_last = rmse_this
    return U, V


U, V = UV_decomposition(user_artist, user_number, artist_number)

