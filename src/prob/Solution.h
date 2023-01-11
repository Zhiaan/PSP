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
    vector<int> sequence;
    double obj1;
    double obj2;
    double obj3;
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
