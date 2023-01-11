//
// Created by Zhiaan on 2023/1/11.
//

#include "Param.h"
using namespace std;

Param::Param(vector<string>& arguments) {
    // Param .
    problemName = "PSP";
    algoName = arguments[1];        // 使用的算法名称
    carNum = arguments[0] == "" ? vector<int>{} : vector<int>{stoi(arguments[0])};      // 测试集车辆数目
    // TODO 改一下读入路径和写出路径
    dataPath = "/Users/l/PSP/data";        // 数据输入路径
    outputPath = "/Users/l/PSP/result/" + algoName + "/" ;         // 结果输出路径
    instancePrefix = "data_";  // 实例前缀
    instanceSuffix = ".csv";  // 实例后缀
    timeLimit = arguments[2] == "" ? INT_MAX : stoi(arguments[1]);  // 求解时间限制
}