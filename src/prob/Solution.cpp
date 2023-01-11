//
// Created by Zhiaan on 2023/1/11.
//

#include "Solution.h"

Solution::Solution(vector<solution> s, instance i, string pathHead) {
    outputResult = s;
    ins = i;
    outputPathHead = pathHead;
}

void Solution::outputCSV() {        // 输出csv
    string outputPath = outputPathHead + ins.instanceNo + ".csv";
    // 保证删除原有文件，重新生成结果
    std::filesystem::remove(outputPath);

    IO io;
    string head = "";
    for(int i = 1; i != stoi(ins.instanceNo.substr(5, ins.instanceNo.size() == 8 ? 3 : 4)) + 1; ++i){
        head += "Variable " + to_string(i) + ",";
    }
    head += "Objective 1,Objective 2,Objective 3";  // 表头
    io.writeCSV(outputPath, head);

    // 表内容
    for(auto solution: outputResult){
        string outputSolution = "";
        for(auto index: solution.sequence){
            outputSolution += to_string(index + 1) + ",";
        }
        outputSolution += to_string(solution.obj1) + "," + to_string(solution.obj2) + "," + to_string(solution.obj3);
        io.writeCSV(outputPath, outputSolution);
    }
}
