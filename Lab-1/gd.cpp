#include <iostream>
#include <stdio.h>
#include <map>
#include <fstream>
#include <vector>
#include <cmath>
#include <cstring>
#include <cstdlib>
#include <sstream>

using namespace std;

int total = 0;  // 训练数据总数
int test_total = 0; // 测试数据总数
int row_number = 0; // 行数
int col_number = 0; // 列数
double epsilon = 0.00001;    // 收敛的阈值
map<int, map<int, int> > M_I, M_J;  // 空间的索引，分别存储某行非空的值、某列非空的值
map<int, map<int, int> > M_test;    // 测试集上的空间索引，用于存储测试数据上某行的空值
const int d = 3;  // 用于矩阵分解的阶数
const char* train_set_path = "./train.txt";
const char* test_set_path = "./test.txt";
const char* output_path_pre = "out-GD-";
const char* output_path_post = ".txt";
double U[150000][d];
double V[1900000][d];

vector<string> split(const string &str, const string &delim)
{
    // 用于字符串分解函数
    vector<string> tokens;
    size_t prev = 0, pos = 0;
    do
    {
        pos = str.find(delim, prev);
        if (pos == string::npos)
            pos = str.length();
        string token = str.substr(prev, pos - prev);
        if (!token.empty())
            tokens.push_back(token);
        prev = pos + delim.length();
    } while (pos < str.length() && prev < str.length());
    return tokens;
}
void init_UV(){
    // 初始化U,V矩阵为随机值
    srand(1);
    for (int i = 0; i < row_number; i++)
    {
        for (int j = 0; j < d; j++)
        {
            U[i][j] = 1;
            // U[i][j] = ((double) rand() / (RAND_MAX));
        }
    }
    for (int i = 0; i < col_number; i++)
    {
        for (int j = 0; j < d; j++)
        {
            V[i][j] = 1;
            // V[i][j] = ((double) rand() / (RAND_MAX));
        }
    }
}
void init()
{
    // 读取训练数据和测试数据,初始化M_I,M_J,M_test
    ifstream file;
    file.open(train_set_path);
    string line;
    map<int, map<int, int> >::iterator it, jt;
    map<int, int>::iterator iit, jjt;
    while (getline(file, line))
    {
        vector<string> ans = split(line, ",");
        int i, j, v;
        i = atoi(ans[0].c_str());
        j = atoi(ans[1].c_str());
        v = atoi(ans[2].c_str());

        total++;
        row_number = max(i, row_number);
        col_number = max(j, col_number);

        it = M_I.find(i);
        if (it != M_I.end())
        {
            it->second.insert(make_pair(j, v));
        }
        else
        {
            map<int, int> temp;
            temp.insert(make_pair(j, v));
            M_I.insert(make_pair(i, temp));
        }

        jt = M_J.find(j);
        if (jt != M_J.end())
        {
            jt->second.insert(make_pair(i, v));
        }
        else
        {
            map<int, int> temp;
            temp.insert(make_pair(i, v));
            M_J.insert(make_pair(j, temp));
        }
    }
    row_number++;
    col_number++;
    cout << "Train Set" << endl;
    cout << row_number << " " << col_number << endl;
    cout << total << endl;
    file.close();

     file.open(test_set_path);
     while (getline(file, line))
     {
         vector<string> ans = split(line, ",");
         int i, j, v;
         i = atoi(ans[0].c_str());
         j = atoi(ans[1].c_str());
         v = atoi(ans[2].c_str());

         test_total++;

         it = M_test.find(i);
         if (it != M_test.end())
         {
             it->second.insert(make_pair(j, v));
         }
         else
         {
             map<int, int> temp;
             temp.insert(make_pair(j, v));
             M_test.insert(make_pair(i, temp));
         }
     }
     file.close();
     cout << "Test Set" << endl;
     cout << test_total << endl;

}

double rmse()
{
    // 计算训练集数据上的RMSE
    double res = 0.0;
    map<int, map<int, int> >::iterator it;
    map<int, int>::iterator jt;
    for (it = M_I.begin(); it != M_I.end(); it++)
    {
        // double p = 0.0;
        int i = it->first;
        for (jt = it->second.begin(); jt != it->second.end(); jt++)
        {
            int j = jt->first;
            int v = jt->second;
            double p = 0;
            for (int k = 0; k < d; k++)
            {
                p += U[i][k] * V[j][k];
            }
            res += (p - v) * (p - v) / total;
        }
    }
    return sqrt(res);
}

double pridict(){
    // 计算测试数据上的RMSE
    double res = 0;
    map<int, map<int, int> >::iterator it = M_test.begin();
    for(;it != M_test.end(); it++){
        int i = it->first;
        for(map<int, int>::iterator jt = it->second.begin(); jt != it->second.end(); jt++){
            int j = jt->first;
            int v = jt->second;
            double p = 0;
            for(int k = 0;k<d;k++){
                p += U[i][k] * V[j][k];
            }
            res += (p - v) * (p - v);
        }
    }
    return sqrt(res / test_total);
}

void GD(){
    // 利用梯度下降来实现矩阵分解
    double lr = 0.01;   // 学习率
    double lamda = 0.01;   // 正则项系数

    int time = 0;
    cout << "lamda = " << lamda << ", lr = " << lr << endl; 
    double rmse_last = rmse();
    cout << time << " " << rmse_last  << ", " << pridict() << endl;
    while(true){
        for(map<int, map<int, int> >::iterator it = M_I.begin(); it != M_I.end(); it++){
            int i = it->first;
            for(map<int, int>::iterator jt = it->second.begin(); jt != it->second.end(); jt++ ){
                int j = jt->first;
                int v = jt->second;

                double error = v;
                for(int k = 0; k < d; k++){
                    error -= U[i][k] * V[j][k];
                }
                for(int k = 0; k < d; k++){
                    U[i][k] += lr * (error * V[j][k] - lamda * U[i][k]);
                }
                for(int k = 0; k < d; k++){
                    V[j][k] += lr * (error * U[i][k] - lamda * V[j][k]);
                }
            }
        }
        double rmse_this = rmse();
        time++;
        cout << time << " " << rmse_this  << ", " << pridict() << endl;
        if(abs(rmse_this - rmse_last) < epsilon)
            break;
        else
            rmse_last = rmse_this;
    }
}

void output()
{
    // 将U,V矩阵输出到文件
    // ofstream outfile;
    // stringstream oss;
    // string path = "output-";
    // string path_back = ".txt";
    // oss << path << d << path_back;
    // const char* p = oss.str().c_str();
    // outfile.open(p, ios::out | ios::trunc);
    // for (int i = 0; i < row_number; i++)
    // {
    //     for (int j = 0; j < d; j++)
    //     {
    //         outfile << U[i][j];
    //         if (j != d - 1)
    //             outfile << " ";
    //     }
    //     outfile << endl;
    // }

    // for (int i = 0; i < d; i++)
    // {
    //     for (int j = 0; j < col_number; j++)
    //     {
    //         outfile << V[i][j];
    //         if (j != col_number - 1)
    //             outfile << " ";
    //     }
    //     outfile << endl;
    // }
    // outfile.close();
    // 以上方式由于服务器上不支持C++11, 所以采用纯C语言的实现方式,如下
    FILE *fp;                                     /* 定义文件指针*/
    char p[30];
    sprintf(p, "%s%d%s", output_path_pre, d, output_path_post);
    if( ( fp = fopen(p, "w") ) == NULL){    /* 打开文件 */
        printf("File open error!\n");
        exit(0);
    }
    for (int i = 0; i < row_number; i++)
    {
        for (int j = 0; j < d; j++)
        {
            fprintf(fp, "%f", U[i][j]);
            if (j != d - 1)
                fprintf(fp, "%s", " ");
        }
        fprintf(fp, "%s", "\n");
    }

    for (int i = 0; i < col_number; i++)
    {
        for (int j = 0; j < d; j++)
        {
            fprintf(fp, "%f", V[i][j]);
            if (j != col_number - 1)
                fprintf(fp, "%s", " ");
        }
        fprintf(fp, "%s", "\n");
    }

    if( fclose( fp ) ){                         /* 关闭文件 */
        printf( "Can not close the file!\n" );
        exit(0);
    }
}

int main()
{
    init();
    // int d_array[10] = {3, 4, 5, 10, 20, 30, 50, 100, 200, 500};
    // for(int i = 0;i<10;i++){
    //     d = d_array[i];
    cout << "Now d = " << d << endl;
    init_UV();
    GD();
    output();
    cout << "test when d = " << d << ", " << pridict() << endl;
    // }
    return 0;
}
