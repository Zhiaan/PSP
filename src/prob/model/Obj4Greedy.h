//
// Created by Zhiaan on 2023/1/23.
//

#ifndef PSP_OBJ4GREEDY_H
#define PSP_OBJ4GREEDY_H
#include <vector>
#include <iostream>
#include "../Solution.h"
#include "../Data.h"
#include <chrono>
#include <algorithm>
#include <cmath>
#include <cassert>
using namespace std;

struct sol{
    vector<int> sequence;
    long long obj1;        // obj1
    long long obj2;        // obj2
    long long obj3;
    long long obj4;        // 总时长
};

class Obj4Greedy {
public:
    Obj4Greedy(instance inst);
    vector<solution> Obj4GreedyRunner();

private:
    instance ins;
    int sequenceLength;
    int neighborSize;
    int maxIterTime;
    sol generateSolution();
    void pretreatSpeedTrans(vector<int> &type2, vector<int> &type4);
    void mutation(sol& solution);
    void swap1(sol& solution);
    void evaluation(sol &c);
    void particallySwapMutation(sol& population);
};


#endif //PSP_OBJ4GREEDY_H
