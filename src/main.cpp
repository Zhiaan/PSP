#include <iostream>
#include <vector>
#include "prob/AlgoRunner.h"
using namespace std;

int main() {
    vector<string> arguments = {"", "greedy", ""};    // 参数：车辆数量 求解模型 求解时间限制
    cout << "Get Start!" << endl;
    AlgoRunner* runner = new AlgoRunner();
    runner->run(arguments);
    delete runner;
    cout << "Over!" << endl;
}
