//
// Created by Zhiaan on 2023/1/11.
//

#ifndef PSP_ALGORUNNER_H
#define PSP_ALGORUNNER_H
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <thread>
#include "Data.h"
#include "../comm/Param.h"
#include "../comm/Param.h"
#include "../comm/IO.h"
#include "Solution.h"
#include <ctime>
#include "model/Greedy.h"
#include "model/ImprovedNSGA2.h"
#include "model/Obj1Greedy.h"
#include <mutex>
#include <set>
using namespace std;
class AlgoRunner {
public:
    vector<solution> chooseAlgo(string algoName, instance i);
    void run(vector<string>&);
    void processFile(Data &d, Param &p, IO &io, const string &filePathName, const int &threadId);
};


#endif //PSP_ALGORUNNER_H
