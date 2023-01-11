//
// Created by Zhiaan on 2023/1/11.
//

#include "AlgoRunner.h"
void AlgoRunner::run(vector<string>& arguments){
    srand((unsigned)time(NULL));    // 用于生成随机数
    Param p(arguments);     // update parameters
    Data d;
    vector<string> filePathList = d.getPathList(p);      // 获得所有测试实例的路径

    vector<instance> instances = d.getInstance(filePathList);   // 获得所有测试实例
    for(auto ins: instances){
        vector<solution> solutions = chooseAlgo(p.algoName, ins);

        // 结果输出
        Solution sol(solutions, ins, p.outputPath);
        sol.outputCSV();
    }
}

vector<solution> AlgoRunner::chooseAlgo(string algoName, instance ins) {
//    clock_t startTime,endTime;  // 记录求解时间
//    startTime = clock();

    vector<solution> result;
    if(algoName == "greedy"){
        Greedy gr(ins);
        solution s = gr.greedyRunner();
        result.emplace_back(s);
    }
    else if(algoName == "NSGA2"){

    }
//    endTime = clock();
//    double spendTime = (double)(endTime - startTime) / CLOCKS_PER_SEC;
//    s.time = spendTime;
    return result;
}