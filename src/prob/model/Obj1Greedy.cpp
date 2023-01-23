//
// Created by Zhiaan on 2023/1/23.
//

#include <random>
#include "Obj1Greedy.h"

Obj1Greedy::Obj1Greedy(instance inst) {
    ins = inst;
    sequenceLength = ins.cars.size();
    neighborSize = 500;
    maxIterTime = 200;
}

vector<solution> Obj1Greedy::Obj1GreedyRunner() {
    vector<solution> solutions;

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
    cout << "insNo:" << ins.instanceNo << ", obj0:" << globalBestSolution.obj0 << ", obj1:" << globalBestSolution.obj1 << endl;
    solutions.emplace_back(solution{globalBestSolution.sequence, globalBestSolution.obj0, globalBestSolution.obj1, 0});
    return solutions;
}

sol Obj1Greedy::generateSolution(){
    sol initSolution;
    do {
        initSolution.sequence.clear();
        // obj1
        vector<int> typeA = {};
        vector<int> typeA4 = {};
        vector<int> typeB = {};
        vector<int> typeB4 = {};

        for (auto i: ins.cars) {
            if (i.type == "A" and i.speedTrans == "两驱") {
                typeA.emplace_back(i.carNo - 1);
            } else if (i.type == "A" and i.speedTrans == "四驱") {
                typeA4.emplace_back(i.carNo - 1);
            } else if (i.type == "B" and i.speedTrans == "两驱") {
                typeB.emplace_back(i.carNo - 1);
            } else if (i.type == "B" and i.speedTrans == "四驱") {
                typeB4.emplace_back(i.carNo - 1);
            }
        }

        pretreatSpeedTrans(typeA, typeA4);
        pretreatSpeedTrans(typeB, typeB4);


        int remainA = typeA.size() % 23;
        int remainB = typeB.size() % 23;
        int fragmentNumA = typeA.size() / 23;
        int fragmentNumB = typeB.size() / 23;

        //        cout << remainA << ' ' << remainB << ' ' << fragmentNumA << ' ' << fragmentNumB << endl;


        int preA = 0, preB = 0, behindA = 0, behindB = 0;
        for (int i = 0; i != fragmentNumA + fragmentNumB; ++i) {      // n1+n2次片段选择
            int typeChoose = ::rand() % 2;      // 选择加入哪个序列
            if (typeChoose == 0 and behindA == typeA.size()) typeChoose = 1;
            if (typeChoose == 1 and behindB == typeB.size()) typeChoose = 0;

            switch (typeChoose) {
                case 0: {        // 选择typeA序列
                    if (typeA.size() - preA < 2 * 23) {
                        initSolution.sequence.insert(initSolution.sequence.end(), typeA.begin() + preA, typeA.end());
                        behindA = typeA.size();
                    } else {
                        int freeLength = remainA == 0 ? 0 : ::rand() % remainA;
                        behindA += 23 + freeLength;
                        initSolution.sequence.insert(initSolution.sequence.end(), typeA.begin() + preA, typeA.begin() + behindA);
                        remainA -= freeLength;
                        preA = behindA;
                    }

                    break;
                }
                case 1: {
                    if (typeB.size() - preB < 2 * 23) {
                        initSolution.sequence.insert(initSolution.sequence.end(), typeB.begin() + preB, typeB.end());
                        behindB = typeB.size();
                    } else {
                        int freeLength = remainB == 0 ? 0 : ::rand() % remainB;

                        //                    cout << freeLength << " B" << " ";
                        behindB += 23 + freeLength;
                        initSolution.sequence.insert(initSolution.sequence.end(), typeB.begin() + preB, typeB.begin() + behindB);
                        remainB -= freeLength;

                        //                    cout << remainA << " rB" << endl;
                        preB = behindB;
                    }
                    break;
                }
            }
        }
        evaluation(initSolution);
    } while (initSolution.obj0 == std::numeric_limits<double>::infinity());
    return initSolution;

}

void Obj1Greedy::swap1(sol& solution) {
    vector<int> parent = solution.sequence;
    int index1 = ::rand() % parent.size();
    int index2 = ::rand() % parent.size();

    swap(parent[index1], parent[index2]);
    sol child = sol{parent, 0, 0};
    evaluation(child);         // 更新child参数
    solution = child.obj0 != 0 ? solution : child;          // 如果为可行解 则保留 否则保留原解
}
void Obj1Greedy::mutation(sol& solution) {
    vector<int> parent = solution.sequence;
    // 生成位置序列（0-1）
//    int oneNum = ::rand() % (parent.size() / 15  );    // 变异位置数量
    int oneNum = 2;
    vector<int> position(parent.size(), 0);
    for(int i = 0; i != oneNum; ++i){
        position[i] = 1;
    }
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    shuffle(position.begin(), position.end(), default_random_engine(seed));   // 对四驱同类型乱序后分组

    // 记录需要变异的元素
    vector<int> genes = {};
    for(int i = 0; i != position.size(); ++i){
        if(position[i] == 1){
            genes.emplace_back(parent[i]);
        }
    }
    seed = std::chrono::system_clock::now().time_since_epoch().count();
    shuffle(genes.begin(), genes.end(), default_random_engine(seed));   // 对四驱同类型乱序后分组

    for(int i = 0, j = 0; j != genes.size(); ++i){      // 重新放回位置
        if(position[i] == 1){
            parent[i] = genes[j++];
        }
    }
    sol child = sol{parent, 0, 0};
    evaluation(child);         // 更新child参数
    solution = child.obj0 != 0 ? solution : child;          // 如果为可行解 则保留 否则保留原解

}

void Obj1Greedy::pretreatSpeedTrans(vector<int> &type2, vector<int> &type4) {
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    shuffle(type4.begin(), type4.end(), default_random_engine(seed));   // 对四驱同类型乱序后分组

    vector<vector<int>> type4Set = {};
    int pre = 0;
    int behind = 0;
    while(behind != type4.size()){
        int length = ::rand() % 3 + 1;
        behind = pre + length > type4.size() ? type4.size() : pre + length;
        vector<int> set(type4.begin()+pre, type4.begin()+behind);

        type4Set.emplace_back(set);
        pre = behind;
    }

    seed = std::chrono::system_clock::now().time_since_epoch().count();
    shuffle(type2.begin(), type2.end(), default_random_engine(seed));         // 对同类型乱序后分组
    vector<int> position(type2.size(), 0);
    for(int i = 0; i != type2.size(); ++i) position[i] = i;

    seed = std::chrono::system_clock::now().time_since_epoch().count();
    shuffle(position.begin(), position.end(), default_random_engine(seed));   // 乱序后选前n个位置插入四驱
    sort(position.begin(), position.begin() + type4Set.size(), [&](const int& a, const int& b) {
        return a > b;});

    for(int i = 0; i != type4Set.size(); ++i){
        type2.insert(type2.begin() + position[i], type4Set[i].begin(), type4Set[i].end());
    }
}

void Obj1Greedy::evaluation(sol &c) {
    c.obj1 = 0;
    c.obj0 = 0;  // obj0和1目标值归零
    vector<int> test = c.sequence;
    sort(test.begin(), test.end());
    vector<int> examine(ins.cars.size());
    for(int i = 0; i != examine.size(); ++i){
        examine[i] = i;
    }
    if(examine != test){
        c.obj1 = -1;
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