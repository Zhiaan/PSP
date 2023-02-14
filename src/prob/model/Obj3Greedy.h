//
// Created by Zhiaan on 2023/2/14.
//

#ifndef PSP_OBJ3GREEDY_H
#define PSP_OBJ3GREEDY_H
#include "Obj4Greedy.h"
#include <random>
class Obj3Greedy {
public:
    Obj3Greedy(instance inst);
    vector<solution> Obj3GreedyRunner();

private:
    instance ins;
    int sequenceLength;
    int neighborSize;
    int maxIterTime;
    sol generateSolution();
    void evaluation(sol &c);
    vector<vector<carInfo>> splitVector(vector<carInfo>);
    void mutation(sol&);

};


#endif //PSP_OBJ3GREEDY_H
