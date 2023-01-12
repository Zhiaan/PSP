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
#include <unordered_map>
struct chromosome{
    vector<int> sequence;
    vector<double> objs;        // {obj1, obj2, obj3, rank}
};

class ImprovedNSGA2 {
public:
    vector<solution> NSGA2Runner();
    ImprovedNSGA2(instance);
    int populationSize;     // 种群大小 500/1000
    int chromosomeLength;   // 染色体长度/基因数量
    static bool cmp(int a, int b);
    static bool cmp1(pair<int, int> a, pair<int, int> b);
private:
    instance ins;
    void evaluation(chromosome &s);
    void randomInitializePopulation(vector<chromosome> &population);
    void pretreatSpeedTrans(vector<int>& type2, vector<int>& type4);
    void greedyObj2InitializePopulation(vector<chromosome> &population);
    void greedyObj1InitializePopulation(vector<chromosome> &population);
    void cross(vector<chromosome>& population);
    void mutation(vector<chromosome>& population);
    void nondominatedSorting(vector<chromosome>& population);
};


#endif //PSP_IMPROVEDNSGA2_H
