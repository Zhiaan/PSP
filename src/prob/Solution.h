//
// Created by Zhiaan on 2023/1/11.
//

#ifndef PSP_SOLUTION_H
#define PSP_SOLUTION_H
#include "../comm/Param.h"
#include "Data.h"
#include "time.h"
#include "../comm/IO.h"
#include <vector>
using namespace std;
struct solution{
    vector<int> sequence;   // 从0开始
    double obj1;        // 焊装切换次数
    double obj2;        // 涂装闲置时间
    double obj3;        // 四驱连续惩罚
    double obj4;        // 总时长
};

class Solution {
private:
    vector<solution> outputResult;
    instance ins;
    string outputPathHead;
public:
    Solution(vector<solution> s, instance i, string pathHead);
    void outputCSV();
};


#endif //PSP_SOLUTION_H
