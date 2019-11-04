# 实验报告

## 数据处理部分
<!-- TODO -->

## 数据划分
<!-- TODO -->
## 矩阵分解部分

主要有以下几种不同的实现方式

1. 使用python实现PPT上公式所表示的矩阵分解
2. 使用C++实现PPT上公式所示的矩阵分解
3. 使用python实现基于SGD的ALS
4. 使用C++实现基于SGD的ALS
5. 使用python实现含有正则系数的ALS

### 关于第一种实现方式

首先基于这个方式的矩阵分解是最早实现的，但是由于其固有的复杂度过高和没有用到numpy等一些矩阵运算的加速工具，
导致每一轮的实际运行时间过长，所以没有实际运行的结果保留。

源文件为`Lab-1.py`，主要函数为`UV_composition`，其用于矩阵分解的计算。

### 关于第二种方式

这种方式相比第一种方式，其主要的不同在于实现细节，即增加了一个空间中的索引，用于记录两种信息：
该行有多少元素以及该列有多少元素。具体的实现示例，`map<int, map<int, int> > M`。
源文件为`Lab-1-server.cpp`。

使用这种方式，对于训练集数据保持预期的结果，当调整参数d时，随着d的增大，对于矩阵M的拟合变好，
整体的RMSE下降；但对于测试数据，无论d取值如何，其RMSE都随着迭代的轮数而增大。

由于实验期间曾询问老师关于以上情况的问题，曾怀疑与训练集、测试集数据划分相关，改变了数据集划分
的方式之后，这种状况仍然没有改变。

### 关于第三种、第四种方式

这两种实现没有本质的区别，细节上也几乎相同，但由于“C++语言速度更快”的原因，在测试时选择了这种
实现方式进行。源文件为服务器上的`/UnderG/zhumingyan/DM/main.cpp`。

在测试的时候，由于SGD这种方式引入了更多的超参数，包括学习率(learning rate, lr)，正则项系数($\lambda$)，
所以需要更多的测试来观察实验结果。

学习率的大小会影响error的计算，进而影响U、V矩阵中的值，由于double数据的存储限制，lr的取值为1e-6，
当盲目调大lr时会导致double类型的上溢出。

整体的测试结果为，在训练集上RMSE始终呈现下降趋势，在测试集数据上RMSE则下降极慢，甚至会出现偶尔的波动上升情况。

### 关于第五种方式

参考[斯坦福大学2015春季cme323课程的第14次笔记][lec14]的基本实现。

[ lec14 ]: http://stanford.edu/~rezab/classes/cme323/S15/notes/lec14.pdf
[ fastals ]: https://web.stanford.edu/~rezab/papers/fastals.pdf

## 参考文献

- [https://web.stanford.edu/~rezab/papers/fastals.pdf][ fastals ]
- [http://stanford.edu/~rezab/classes/cme323/S15/notes/lec14.pdf][ lec14 ]
