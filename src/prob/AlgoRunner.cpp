//
// Created by Zhiaan on 2023/1/11.
//

#include "AlgoRunner.h"
void AlgoRunner::run(vector<string>& arguments){
    srand((unsigned)time(NULL));    // 用于生成随机数
    Param p(arguments);     // update parameters
    Data d;
    vector<string> filePathList = d.getPathList(p);      // 获得所有测试实例的路径

//    for (const auto &filePath: filePathList) {
// TODO 先处理一个测试实例
        string filePath = p.dataPath + "/data_103.csv";
        cout << filePath << endl;
        instance ins = d.getInstance(filePath);

        vector<solution> solutions = chooseAlgo(p.algoName, ins);

        // 结果输出
        Solution sol(solutions, ins, p.outputPath);
        sol.outputCSV();
//    }
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
    else if(algoName == "INSGA2"){
        ImprovedNSGA2 INSGA2(ins);
        result = INSGA2.NSGA2Runner();
    }
//    endTime = clock();
//    double spendTime = (double)(endTime - startTime) / CLOCKS_PER_SEC;
//    s.time = spendTime;
    return result;
}