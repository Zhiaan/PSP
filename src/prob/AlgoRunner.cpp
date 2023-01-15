//
// Created by Zhiaan on 2023/1/11.
//

#include "AlgoRunner.h"
void AlgoRunner::run(vector<string>& arguments){
    srand((unsigned)time(NULL));    // 用于生成随机数
    Param p(arguments);     // update parameters
    Data d;
    vector<string> filePathList = d.getPathList(p);      // 获得所有测试实例的路径

    IO io;
    for (const auto &filePath: filePathList) {
//        string filePath = p.dataPath + "/data_103.csv";
        instance ins = d.getInstance(filePath);


        vector<solution> solutions = chooseAlgo(p.algoName, ins);

        // 结果分别输出
        Solution sol(solutions, ins, p.outputPath);
        sol.outputCSV();

        // 结果总输出
        string statisticsPath = p.outputPath + "statistics.csv";
        cout << statisticsPath << endl;
        double avg1 = 0, avg2 = 0, avg3 = 0;
        double sum1 = 0, sum2 = 0, sum3 = 0;
        for(auto& s: solutions){
            sum1 += s.obj1;
            sum2 += s.obj2;
            sum3 += s.obj3;
        }
        avg1 = sum1 / solutions.size();
        avg2 = sum2 / solutions.size();
        avg3 = sum3 / solutions.size();
        string outputSolution = ins.instanceNo + "," + to_string(avg1) + "," + to_string(avg2) + "," + to_string(avg3) ;
        io.writeCSV(statisticsPath, outputSolution);
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
    else if(algoName == "INSGA2"){
        ImprovedNSGA2 INSGA2(ins);
        result = INSGA2.NSGA2Runner();
    }
//    endTime = clock();
//    double spendTime = (double)(endTime - startTime) / CLOCKS_PER_SEC;
//    s.time = spendTime;
    return result;
}