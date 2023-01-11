//
// Created by Zhiaan on 2023/1/12.
//

#ifndef PSP_IMPROVEDNSGA2_H
#define PSP_IMPROVEDNSGA2_H
#include "../Data.h"
#include <iostream>
#include <algorithm>
#include "../Solution.h"
#include "Greedy.h"
struct chromosome{
    vector<int> sequence;
    vector<double> objs;
};
class ImprovedNSGA2 {
public:
    vector<solution> NSGA2Runner();
    ImprovedNSGA2(instance);
    int populationSize;     // 种群大小 500/1000
    int chromosomeLength;   // 染色体长度/基因数量
private:
    instance ins;
    void evaluation(chromosome &s);
    void initializePopulation(vector<chromosome>&);
};


#endif //PSP_IMPROVEDNSGA2_H
