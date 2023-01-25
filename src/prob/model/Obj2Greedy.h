//
// Created by Zhiaan on 2023/1/25.
//

#ifndef PSP_OBJ2GREEDY_H
#define PSP_OBJ2GREEDY_H

#include <vector>
#include <iostream>
#include "../Solution.h"
#include "../Data.h"
#include <chrono>
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <set>
#include "Obj1Greedy.h"
using namespace std;

class Obj2Greedy {
public:
    Obj2Greedy(instance inst);
    vector<solution> Obj2GreedyRunner();

private:
    instance ins;
    int sequenceLength;
    int neighborSize;
    int maxIterTime;
    int obj1GreedySolutionObj2;
    sol generateSolution();
    void pretreatSpeedTrans(vector<int> &type2, vector<int> &type4);
    void mutation(sol& solution);
    void swap1(sol& solution);
    void evaluation(sol &c);
};



#endif //PSP_OBJ2GREEDY_H
