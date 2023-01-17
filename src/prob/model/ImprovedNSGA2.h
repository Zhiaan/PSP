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
#include "cmath"
#include <chrono>
struct chromosome{
    vector<int> sequence;
    vector<double> objs;        // {obj1, obj2, obj3}
    int rank;
    double crowding_distance;

    int populationIndex;

    friend bool operator <(const chromosome& a, const chromosome& b) {
        bool flag = (a.objs[0] <= b.objs[0]) && (a.objs[1] <= b.objs[1]) && (a.objs[2] <= b.objs[2]);
        bool obj1_le = a.objs[0] < b.objs[0];
        bool obj2_le = a.objs[1] < b.objs[1];
        bool obj3_le = a.objs[2] < b.objs[2];
        bool le = obj1_le || obj2_le || obj3_le;
        return le && flag;
    }
};

class ImprovedNSGA2 {
public:
    vector<solution> NSGA2Runner();
    ImprovedNSGA2(instance);
    int populationSize;     // 种群大小 500/1000
    int chromosomeLength;   // 染色体长度/基因数量
    int maxIter;        // 最大迭代次数
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
