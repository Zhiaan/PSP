//
// Created by Zhiaan on 2023/1/23.
//

#ifndef PSP_OBJ1GREEDY_H
#define PSP_OBJ1GREEDY_H
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
    double obj0;        // obj0
    double obj1;        // obj1
    double obj2;
    double time;        // 总时长
};

class Obj1Greedy {
public:
    Obj1Greedy(instance inst);
    vector<solution> Obj1GreedyRunner();

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


#endif //PSP_OBJ1GREEDY_H
