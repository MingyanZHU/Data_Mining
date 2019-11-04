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

int total = 0;
int test_total = 0;
int row_number = 0;
int col_number = 0;
double epsilon = 0.001;
map<int, map<int, int> > M_I, M_J;
map<int, map<int, int> > M_test;
const char* train_set_path = "./train.txt";
const char* test_set_path = "./test.txt";
const char* output_path_pre = "out-ALS-";
const char* output_path_post = ".txt";
int d = 3;
double U[150000][1000];
double V[1000][1900000];

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
    srand(1);
    for (int i = 0; i < row_number; i++)
        {
            for (int j = 0; j < d; j++)
            {
                U[i][j] = 1;
                // U[i][j] = ((double) rand() / (RAND_MAX));
            }
        }
        for (int i = 0; i < d; i++)
        {
            for (int j = 0; j < col_number; j++)
            {
                V[i][j] = 1;
                // V[i][j] = ((double) rand() / (RAND_MAX));
            }
        }
}
void init()
{
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
                p += U[i][k] * V[k][j];
            }
            res += (p - v) * (p - v);
        }
    }
    return sqrt(res / total);
}

double pridict(){
    double res = 0;
    map<int, map<int, int> >::iterator it = M_test.begin();
    for(;it != M_test.end(); it++){
        int i = it->first;
        for(map<int, int>::iterator jt = it->second.begin(); jt != it->second.end(); jt++){
            int j = jt->first;
            int v = jt->second;
            double p = 0;
            for(int k = 0;k<d;k++){
                p += U[i][k] * V[k][j];
            }
            res += (p - v) * (p - v);
        }
    }
    return sqrt(res / test_total);
}

void UV_decomposition()
{
    int time = 0;
    double rmse_last = rmse();
    cout << time << " " << rmse_last  << ", " << pridict() << endl;
    while (true)
    {
        time++;
        bool u_turn = true;
        int ui = 0;
        int uj = 0;
        int vi = 0;
        int vj = 0;
        bool u_finished = false;
        bool v_finished = false;

        while (1)
        {
            if (u_finished && v_finished)
                break;
            if (v_finished || u_turn)
            {
                u_turn = false;
                double numerator = 0;
                double denominator = 0;
                map<int, map<int, int> >::iterator it = M_I.find(ui);
                if (it != M_I.end())
                {
                    // int j = it->first;
                    // assert(j == ui);
                    for (map<int, int>::iterator jt = it->second.begin(); jt != it->second.end(); jt++)
                    {
                        int j = jt->first;
                        int v = jt->second;
                            double p = 0.0;
                        for (int k = 0; k < d; k++)
                        {
                            if (k != uj)
                            {
                                p += U[ui][k] * V[k][j];
                            }
                        }
                        numerator += V[uj][j] * (v - p);
                        denominator += V[uj][j] * V[uj][j];
                    }
		            double temp = numerator / denominator;
                    if(!isinf(temp) && !isnan(temp)){
                        U[ui][uj] = temp;
                    }
                //    U[ui][uj] = numerator / denominator;
                }
                uj++;
                if (uj >= d)
                {
                    ui++;
                    uj = 0;
                    if (ui >= row_number)
                    {
                        u_finished = true;
                    }
                }
            }
            else if (u_finished || !u_turn)
            {
                u_turn = true;
                double numerator = 0;
                double denominator = 0;
                map<int, map<int, int> >::iterator jt = M_J.find(vj);
                if (jt != M_J.end())
                {
                    // int i = jt->first;
                    for (map<int, int>::iterator it = jt->second.begin(); it != jt->second.end(); it++)
                    {
                        int i = it->first;
                        int v = it->second;
                        double p = 0.0;
                        for (int k = 0; k < d; k++)
                        {
                            if (k != vi)
                            {
                                p += U[i][k] * V[k][vj];
                            }
                        }
                        numerator += U[i][vi] * (v - p);
                        denominator += U[i][vi] * U[i][vi];
                    }
		            double temp = numerator / denominator;
                    if(!isinf(temp) && !isnan(temp)){
                        V[vi][vj] = temp;
                    }
                   // V[vi][vj] = numerator / denominator;
                }
                vj++;
                if (vj >= col_number)
                {
                    vi++;
                    vj = 0;
                    if (vi >= d)
                    {
                        v_finished = true;
                    }
                }
            }
        }
        double rmse_this = rmse();
        cout << time << " " << rmse_this  << ", " << pridict() << endl;
        if (abs(rmse_last - rmse_this) < epsilon)
            break;
        else
            rmse_last = rmse_this;

        if (time > 100)
        {
            break;
        }
    }
}

void output()
{
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

    for (int i = 0; i < d; i++)
    {
        for (int j = 0; j < col_number; j++)
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
    // map<int, map<int, int>>::iterator it;
    // it = M_I.find(1);
    init();
    int d_array[10] = {3, 4, 5, 10, 20, 30, 50, 100, 200, 500};
    for(int i = 0;i<10;i++){
        d = d_array[i];
        cout << "Now d = " << d << endl;
        init_UV();
        UV_decomposition();
        output();
        cout << "test when d = " << d << ", " << pridict() << endl;
    }
    return 0;
}
