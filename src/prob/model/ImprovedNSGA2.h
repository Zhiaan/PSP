//
// Created by Zhiaan on 2023/1/12.
//

#ifndef PSP_IMPROVEDNSGA2_H
#define PSP_IMPROVEDNSGA2_H
#include "../Data.h"
#include "../Solution.h"
#include <iostream>
#include <algorithm>
#include <unordered_map>
#include "cmath"
#include <chrono>
#include <random>
#include <cassert>
#include <set>
#include <unordered_set>
#include "Obj2Greedy.h"
#include "Obj3Greedy.h"
#include "Obj4Greedy.h"
struct chromosome{
    vector<int> sequence;
    vector<long long> objs;        // {obj1, obj2, obj3, obj4}
    int rank;
    double crowding_distance;

    int populationIndex;

    friend bool operator <(const chromosome& a, const chromosome& b) {
        // 判断所有 objs <=
        bool flag_1 = true;
        for (int i = 0; i < a.objs.size(); ++i) {
            if (a.objs[i] > b.objs[i]) {
                flag_1 = false;
                break;
            }
        }
        // 判断存在 objs <
        bool flag_2 = false;
        for (int i = 0; i < a.objs.size(); ++i) {
            if (a.objs[i] < b.objs[i]) {
                flag_2 = true;
                break;
            }
        }
        return flag_1 && flag_2;
    }
};

class ImprovedNSGA2 {
public:
    vector<solution> NSGA2Runner();
    ImprovedNSGA2(instance);
    int populationSize;     // 种群大小 500/1000
    int chromosomeLength;   // 染色体长度/基因数量
    int maxIter;        // 最大迭代次数
    int iter;       // 目前迭代次数
    int crossTime;  // cross重复次数
    static bool cmp(int a, int b);
    static bool cmp1(pair<int, int> a, pair<int, int> b);
    Obj2Greedy og2;
    Obj3Greedy og3;
    Obj4Greedy og4;
private:
    instance ins;
    void evaluation(chromosome &s);
    void randomInitializePopulation(vector<chromosome> &population);
    void pretreatSpeedTrans(vector<int>& type2, vector<int>& type4);
    void greedyObj1InitializePopulation(vector<chromosome> &population);
    void greedyObj2InitializePopulation(vector<chromosome> &population);
    void greedySortInitializePopulation(vector<chromosome> &population);
    void cross(vector<chromosome>& population);
    void particallyMappedCross(vector<chromosome>& population);
    void colorCommonCross(vector<chromosome>& population);
    void mutation(vector<chromosome>& population);
    void particallySwapMutation(vector<chromosome>& population);
    void nondominatedSorting(vector<chromosome>& population);
    void computeCrowdingDistance(vector<chromosome> &population, vector<vector<chromosome>> &fronts);

    // helper func
    void findStartEnd(const vector<int> &sequence, int &start, int &end) const;
    bool checkObjBetter(chromosome &c);
    vector<long long> rank0_best_obj; // current iter rank0 best
};


#endif //PSP_IMPROVEDNSGA2_H
