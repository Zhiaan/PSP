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

    const unsigned int numThreads = std::thread::hardware_concurrency();
//    const unsigned int numThreads = 1;
    std::vector<std::thread> pool;
    const int numFiles = filePathList.size();
//    const int numFiles = 1;
    int fileId = 0;
    std::mutex mtx;

    for (int threadId = 0; threadId < numThreads; ++threadId) {
        std::thread currentThread([&](const int threadId) {
            mtx.lock();
            while (fileId < numFiles) {
                string filePath = filePathList[fileId];
//                string filePath = p.dataPath + "/data_103.csv";
                ++fileId;
                mtx.unlock();

                printf("thread id %d, file name %s\n", threadId, filePath.c_str());
                processFile(d, p, io, filePath, threadId);

                mtx.lock();
            }
            mtx.unlock();
        }, threadId);
        pool.push_back(std::move(currentThread));
    } 

    for (auto& t : pool) {
        t.join();
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
    else if(algoName == "Obj1Greedy"){
        Obj1Greedy og(ins);
        result = og.Obj1GreedyRunner();
    }
//    endTime = clock();
//    double spendTime = (double)(endTime - startTime) / CLOCKS_PER_SEC;
//    s.time = spendTime;
    return result;
}

void AlgoRunner::processFile(Data &d, Param &p, IO &io, const string &filePathName, const int &threadId) {
//  string filePath = p.dataPath + "/data_103.csv";
    instance ins = d.getInstance(filePathName);
    ins.threadId = threadId;

    vector<solution> solutions = chooseAlgo(p.algoName, ins);
    // 结果分别输出
    Solution sol(solutions, ins, p.outputPath);
    sol.outputCSV();

    // 结果总输出
    string statisticsPath = p.outputPath + "statistics.csv";
    //    double avg1 = 0, avg2 = 0, avg3 = 0;
//    double sum1 = 0, sum2 = 0, sum3 = 0;
//    for(auto& s: solutions){
//        sum1 += s.obj1;
//        sum2 += s.obj2;
//        sum3 += s.obj3;
//    }
//    avg1 = sum1 / solutions.size();
//    avg2 = sum2 / solutions.size();
//    avg3 = sum3 / solutions.size();
    string outputSolution;
    for(auto& s: solutions){
        if(s.obj1 == 0){
            outputSolution = ins.instanceNo + "," + to_string(s.obj1) + "," + to_string(s.obj2) + "," + to_string(s.obj3);
            break;
        }
    }
    io.writeCSV(statisticsPath, outputSolution);
}