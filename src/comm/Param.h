//
// Created by Zhiaan on 2023/1/11.
//

#ifndef PSP_PARAM_H
#define PSP_PARAM_H
#include <string>
#include <vector>
using namespace std;
class Param {
public:
    string problemName;     // 问题名称
    string algoName;        // 使用的算法名称
    vector<int> carNum;    // 车辆数量
    string dataPath;        // 数据输入路径
    string outputPath;      // 输出数据
    string instancePrefix;  // 实例前缀
    string instanceSuffix;  // 实例后缀

    // 算法参数写在这里
    int timeLimit;          // 求解时长限制

    Param(vector<string>&);
};

#endif //PSP_PARAM_H
