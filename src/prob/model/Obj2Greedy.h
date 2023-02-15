//
// Created by Zhiaan on 2023/2/15.
//

#ifndef PSP_OBJ2GREEDY_H
#define PSP_OBJ2GREEDY_H

#include "Obj4Greedy.h"
#include <random>
class Obj2Greedy {
public:
    Obj2Greedy(instance inst);
    vector<solution> obj2GreedyRunner(vector<int> sequence);

private:
    instance ins;
    int sequenceLength;
    int neighborSize;
    int maxIterTime;
    sol existSolution;
    sol generateSolution();
    void evaluation(sol &c);
    void particallySwapMutation(sol&);
};


#endif //PSP_OBJ2GREEDY_H
