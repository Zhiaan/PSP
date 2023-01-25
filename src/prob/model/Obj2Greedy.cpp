//
// Created by Zhiaan on 2023/1/25.
//

#include <random>
#include "Obj2Greedy.h"

Obj2Greedy::Obj2Greedy(instance inst) {
    ins = inst;
    sequenceLength = ins.cars.size();
    neighborSize = 500;
    maxIterTime = 200;
    string filePath1 = "../result/Obj1Greedy/" + ins.instanceNo + ".csv";
    IO io;
    vector<vector<string>> obj1GreedySolution = io.readCSV(filePath1);
    obj1GreedySolutionObj2 = stoi(obj1GreedySolution[0][sequenceLength + 1]);
}

vector<solution> Obj2Greedy::Obj2GreedyRunner() {
    vector<solution> solutions;
    set<vector<int>> noRepetSolutions;
    sol globalBestSolution;
    sol localBestSolution;
    localBestSolution = generateSolution();
    globalBestSolution = localBestSolution;

//    for(int i = 0; i != maxIterTime; ++i){
    int flag = 0;
    while(true){
        printf("current threadId: %d, current instance: %s, flag: %d\n", ins.threadId, ins.instanceNo.c_str(), flag);
        sol bestNeighbor;
        bestNeighbor.obj1 = std::numeric_limits<double>::infinity();
        for(int j = 0; j != neighborSize; ++j){
            sol neighbor = localBestSolution;
//            mutation(neighbor);
            swap1(neighbor);
            if(neighbor.obj1 < bestNeighbor.obj1)   bestNeighbor = neighbor;

//            cout << neighbor.obj1 << ' ' << bestNeighbor.obj1 << endl;
        }
        localBestSolution = bestNeighbor;
        if(localBestSolution.obj1 < obj1GreedySolutionObj2){
            noRepetSolutions.insert(localBestSolution.sequence);
        }
        if(bestNeighbor.obj1 < globalBestSolution.obj1) {
            globalBestSolution = bestNeighbor;
            flag = 0;
        }
        else{
            ++flag;
        }
//        cout << localBestSolution.obj1 << ' ' << globalBestSolution.obj1 << endl;
//        cout << "-----------------" << endl;
        if(flag == maxIterTime) break;
    }

    solutions.emplace_back(solution{globalBestSolution.sequence, globalBestSolution.obj0, globalBestSolution.obj1, 0});
    for(auto i: noRepetSolutions){
        sol temp = sol{i, 0, 0};
        evaluation(temp);
        solutions.emplace_back(solution{temp.sequence, temp.obj0, temp.obj1});
    }
    return solutions;
}

sol Obj2Greedy::generateSolution(){
    sol initSolution;
    label:
    initSolution.sequence = vector<int>(sequenceLength, 0);
    for(int index = 0; index != sequenceLength; ++index){
        initSolution.sequence[index] = index;
    }
    // 随机生成初序列
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    shuffle (initSolution.sequence.begin(), initSolution.sequence.end(), std::default_random_engine(seed));

    // 计算目标值
    initSolution.obj0 = 0;
    initSolution.obj1 = 0;
    evaluation(initSolution);
    if(initSolution.obj0 == std::numeric_limits<double>::infinity())  goto label;     // 生成可行解 不可行解重新生成

    return initSolution;

}

void Obj2Greedy::swap1(sol& solution) {
    vector<int> parent = solution.sequence;
    int index1 = ::rand() % parent.size();
    int index2 = ::rand() % parent.size();

    swap(parent[index1], parent[index2]);
    sol child = sol{parent, 0, 0};
    evaluation(child);         // 更新child参数
    solution = child.obj0 == std::numeric_limits<double>::infinity() ? solution : child;          // 如果为可行解 则保留 否则保留原解
}
void Obj2Greedy::mutation(sol& solution) {
    vector<int> parent = solution.sequence;
    // 生成位置序列（0-1）
//    int oneNum = ::rand() % (parent.size() / 15  );    // 变异位置数量
    int oneNum = 2;
    vector<int> position(parent.size(), 0);
    for(int i = 0; i != oneNum; ++i){
        position[i] = 1;
    }
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    shuffle(position.begin(), position.end(), default_random_engine(seed));

    // 记录需要变异的元素
    vector<int> genes = {};
    for(int i = 0; i != position.size(); ++i){
        if(position[i] == 1){
            genes.emplace_back(parent[i]);
        }
    }
    seed = std::chrono::system_clock::now().time_since_epoch().count();
    shuffle(genes.begin(), genes.end(), default_random_engine(seed));

    for(int i = 0, j = 0; j != genes.size(); ++i){      // 重新放回位置
        if(position[i] == 1){
            parent[i] = genes[j++];
        }
    }
    sol child = sol{parent, 0, 0};
    evaluation(child);         // 更新child参数
    solution = child.obj0 != 0 ? solution : child;          // 如果为可行解 则保留 否则保留原解

}

void Obj2Greedy::evaluation(sol &c) {
    c.obj1 = 0;
    c.obj0 = 0;  // obj0和1目标值归零
    vector<int> test = c.sequence;
    sort(test.begin(), test.end());
    vector<int> examine(ins.cars.size());
    for(int i = 0; i != examine.size(); ++i){
        examine[i] = i;
    }
    if(examine != test){
        c.obj0 = c.obj1 = -1;
        cout << "error" << endl;
        return;
    }


    int typeCommonTime = 1;
    int colorCommonTime = 0;
    int speedTransCommonTime = 0;   // 连续四驱次数
    for(int i = 0; i != c.sequence.size() - 1; ++i){
        // 记录四驱连续次数
        if(ins.cars[c.sequence[i]].speedTrans == "四驱"){
            ++speedTransCommonTime;
            if(speedTransCommonTime == 4){  // 连续四驱数量到4 将目标值设为极大值
                c.obj0 = c.obj1 = std::numeric_limits<double>::infinity();
                goto label;
            }
        }
        else{
            speedTransCommonTime = 0;
        }

        // obj1 焊装总等待时间
        if(ins.cars[c.sequence[i]].type == ins.cars[c.sequence[i + 1]].type){   // 如果前后相等 记录连续相等车数
            ++typeCommonTime;
        }
        else{
            if(typeCommonTime < ins.weldingContinueTime / ins.weldingTime){     // 如果前后不相等且小于30min
                c.obj0 += ins.weldingContinueTime - typeCommonTime * ins.weldingTime;
            }
            typeCommonTime = 1;
        }

        // obj2 切换喷头次数
        if(ins.cars[c.sequence[i]].roofColor != "无对比颜色" and ins.cars[c.sequence[i]].roofColor != ins.cars[c.sequence[i]].bodyColor){     // 本车车顶颜色!=车身颜色
            ++c.obj1;
        }
        else{               // 本车车顶颜色=车身颜色
            ++colorCommonTime;
            if(colorCommonTime == 5){       // 如果五辆车颜色相同
                ++c.obj1;
                colorCommonTime = 0;
                continue;
            }

        }
        // 如果前车车身!=后车车顶
        if(ins.cars[c.sequence[i]].bodyColor != (ins.cars[c.sequence[i + 1]].roofColor == "无对比颜色" ? ins.cars[c.sequence[i + 1]].bodyColor : ins.cars[c.sequence[i + 1]].roofColor)){
            ++c.obj1;
            colorCommonTime = 0;
        }

    }
    if(speedTransCommonTime == 3 and ins.cars[*(c.sequence.end()-1)].speedTrans == "四驱"){
        c.obj0 = c.obj1 = std::numeric_limits<double>::infinity();
        goto label;
    }
    if(ins.cars[*(c.sequence.end()-1)].roofColor != "无对比颜色" and ins.cars[*(c.sequence.end()-1)].roofColor != ins.cars[*(c.sequence.end()-1)].bodyColor){     // 最后一辆车的车顶颜色!=车身颜色
        ++c.obj1;
    }
    c.obj1 *= 80;

    label:
    return ;

}
