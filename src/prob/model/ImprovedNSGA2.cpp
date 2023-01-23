//
// Created by Zhiaan on 2023/1/12.
//

#include <random>
#include "ImprovedNSGA2.h"
ImprovedNSGA2::ImprovedNSGA2(instance inst) {
    ins = inst;
    populationSize = 500;
    chromosomeLength = ins.cars.size();
    maxIter = 1000;
}

vector<solution> ImprovedNSGA2::NSGA2Runner() {
    vector<solution> solutions;

    vector<chromosome> population(populationSize);

    // 生成初始解
    greedyObj2InitializePopulation(population);     // obj2贪婪算法初始化种群
//    randomInitializePopulation(population);           // 随机初始化种群

    for(int iter = 0; iter < maxIter; ++iter){
        printf("current threadId: %d, current instance: %s, current iter: %d\n", ins.threadId, ins.instanceNo.c_str(), iter);
        vector<chromosome> newPopulation = population;      // 生成新种群
        cross(newPopulation, iter);                           // 交叉算子

        mutation(newPopulation, iter);                        // 变异算子

        newPopulation.insert(newPopulation.end(), population.begin(), population.end());    // 老种群与新种群合并

        nondominatedSorting(newPopulation);     // 非支配排序

        population.clear();
        population.insert(population.begin(), newPopulation.begin(), newPopulation.begin() + populationSize);

    }

    for(auto i: population) {
        solution s;
        s.sequence = i.sequence;
        s.obj1 = i.objs[0];
        s.obj2 = i.objs[1];
        s.obj3 = i.objs[2];
        solutions.emplace_back(s);
        cout << i.objs[0] << ' ' << i.objs[1] << ' ' << i.objs[2] << ' ' << i.rank  << ' ' << i.crowding_distance  << endl;
        if(i.rank != 0 or solutions.size() == 50) break;
        // for (auto j: i.sequence) {
        //     cout << j << ' ';
        // } cout << endl;
    }
    cout << population.size() << ' ' << solutions.size() <<  endl;

    return solutions;
}

void ImprovedNSGA2::evaluation(chromosome &c) {
    c.objs = vector<double>(3, 0);  // 三目标值归零
    vector<int> test = c.sequence;
    sort(test.begin(), test.end());
    vector<int> examine(ins.cars.size());
    for(int i = 0; i != examine.size(); ++i){
        examine[i] = i;
    }
    if(examine != test){
        c.objs[0] = c.objs[1] = c.objs[2] = -1;
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
                c.objs[0] = c.objs[1] = c.objs[2] = std::numeric_limits<double>::infinity();
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
                c.objs[0] += ins.weldingContinueTime - typeCommonTime * ins.weldingTime;
            }
            typeCommonTime = 1;
        }

        // obj2 切换喷头次数
        if(ins.cars[c.sequence[i]].roofColor != "无对比颜色" and ins.cars[c.sequence[i]].roofColor != ins.cars[c.sequence[i]].bodyColor){     // 本车车顶颜色!=车身颜色
            ++c.objs[1];
        }
        else{               // 本车车顶颜色=车身颜色
            ++colorCommonTime;
            if(colorCommonTime == 5){       // 如果五辆车颜色相同
                ++c.objs[1];
                colorCommonTime = 0;
                continue;
            }

        }
        // 如果前车车身!=后车车顶
        if(ins.cars[c.sequence[i]].bodyColor != (ins.cars[c.sequence[i + 1]].roofColor == "无对比颜色" ? ins.cars[c.sequence[i + 1]].bodyColor : ins.cars[c.sequence[i + 1]].roofColor)){
            ++c.objs[1];
            colorCommonTime = 0;
        }

    }
    if(speedTransCommonTime == 3 and ins.cars[*(c.sequence.end()-1)].speedTrans == "四驱"){
        c.objs[0] = c.objs[1] = c.objs[2] = std::numeric_limits<double>::infinity();
        goto label;
    }
    if(ins.cars[*(c.sequence.end()-1)].roofColor != "无对比颜色" and ins.cars[*(c.sequence.end()-1)].roofColor != ins.cars[*(c.sequence.end()-1)].bodyColor){     // 最后一辆车的车顶颜色!=车身颜色
        ++c.objs[1];
    }
    c.objs[1] *= 80;

    // obj3
    c.objs[2] += c.objs[0] + ins.weldingTime * c.sequence.size() + c.objs[1] + ins.paintingTime * 2 * c.sequence.size() + ins.assembleTime * c.sequence.size();
    label:
    return ;

}

void ImprovedNSGA2::randomInitializePopulation(vector<chromosome>& population) {
    for(auto& chro: population){
        label:
        chro.sequence = vector<int>(chromosomeLength, 0);
        for(int index = 0; index != chromosomeLength; ++index){
            chro.sequence[index] = index;
        }
        // 随机生成初序列
        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        shuffle (chro.sequence.begin(), chro.sequence.end(), std::default_random_engine(seed));

        // 计算目标值
        chro.objs = vector<double>(3, 0);
        evaluation(chro);
        if(chro.objs[0] == std::numeric_limits<double>::infinity())  goto label;     // 生成可行解 不可行解重新生成

    }
}

void ImprovedNSGA2::greedyObj2InitializePopulation(vector<chromosome> &population){
    string filePath = "../result/Obj1Greedy/" + ins.instanceNo + ".csv";
    // TODO 地址
    IO io;
    vector<vector<string>> greedySolution = io.readCSV(filePath);
    for(int i = 0; i != populationSize; ++i){
        if(i == 0){
            for(int j = 0; j != chromosomeLength; ++j){
                population[i].sequence.emplace_back(stoi(greedySolution[0][j]));
            }
            evaluation(population[i]);
        }
        else    population[i] = population[0];
    }


//    for(auto& chro: population) {
//        do {
//            chro.sequence.clear();
//            // obj1
//            vector<int> typeA = {};
//            vector<int> typeA4 = {};
//            vector<int> typeB = {};
//            vector<int> typeB4 = {};
//
//            for (auto i: ins.cars) {
//                if (i.type == "A" and i.speedTrans == "两驱") {
//                    typeA.emplace_back(i.carNo - 1);
//                } else if (i.type == "A" and i.speedTrans == "四驱") {
//                    typeA4.emplace_back(i.carNo - 1);
//                } else if (i.type == "B" and i.speedTrans == "两驱") {
//                    typeB.emplace_back(i.carNo - 1);
//                } else if (i.type == "B" and i.speedTrans == "四驱") {
//                    typeB4.emplace_back(i.carNo - 1);
//                }
//            }
//
//            pretreatSpeedTrans(typeA, typeA4);
//            pretreatSpeedTrans(typeB, typeB4);
//
//
//            int remainA = typeA.size() % 23;
//            int remainB = typeB.size() % 23;
//            int fragmentNumA = typeA.size() / 23;
//            int fragmentNumB = typeB.size() / 23;
//
//            //        cout << remainA << ' ' << remainB << ' ' << fragmentNumA << ' ' << fragmentNumB << endl;
//
//
//            int preA = 0, preB = 0, behindA = 0, behindB = 0;
//            for (int i = 0; i != fragmentNumA + fragmentNumB; ++i) {      // n1+n2次片段选择
//                int typeChoose = ::rand() % 2;      // 选择加入哪个序列
//                if (typeChoose == 0 and behindA == typeA.size()) typeChoose = 1;
//                if (typeChoose == 1 and behindB == typeB.size()) typeChoose = 0;
//
//                switch (typeChoose) {
//                    case 0: {        // 选择typeA序列
//                        if (typeA.size() - preA < 2 * 23) {
//                            chro.sequence.insert(chro.sequence.end(), typeA.begin() + preA, typeA.end());
//                            behindA = typeA.size();
//                        } else {
//                            int freeLength = remainA == 0 ? 0 : ::rand() % remainA;
//                            behindA += 23 + freeLength;
//                            chro.sequence.insert(chro.sequence.end(), typeA.begin() + preA, typeA.begin() + behindA);
//                            remainA -= freeLength;
//                            preA = behindA;
//                        }
//
//                        break;
//                    }
//                    case 1: {
//                        if (typeB.size() - preB < 2 * 23) {
//                            chro.sequence.insert(chro.sequence.end(), typeB.begin() + preB, typeB.end());
//                            behindB = typeB.size();
//                        } else {
//                            int freeLength = remainB == 0 ? 0 : ::rand() % remainB;
//
//                            //                    cout << freeLength << " B" << " ";
//                            behindB += 23 + freeLength;
//                            chro.sequence.insert(chro.sequence.end(), typeB.begin() + preB, typeB.begin() + behindB);
//                            remainB -= freeLength;
//
//                            //                    cout << remainA << " rB" << endl;
//                            preB = behindB;
//                        }
//                        break;
//                    }
//                }
//            }
//            evaluation(chro);
//        } while (chro.objs[0] == std::numeric_limits<double>::infinity());
//
////        cout << chro1.sequence.size() << endl;
////        //    for(auto i: chro1.sequence){
////        //        cout << i << ' ';
////        //    }cout << endl;
////        cout << chro1.objs[0] << ' ' << chro1.objs[1] << ' ' << chro1.objs[2] << endl;
////        cout << "---------------" << endl;
//    }
}

void ImprovedNSGA2::greedyObj1InitializePopulation(vector<chromosome> &population){
    //    chromosome chro;
//    unordered_map<string, vector<int>> pureColorCars;
//    vector<int> mixedColorCars = {};
//    for(auto i: ins.cars){
//        if(i.roofColor == i.bodyColor or i.roofColor == "无对比颜色"){
//            unordered_map<string, vector<int>>::iterator iter;
//            if((iter = pureColorCars.find(i.bodyColor)) != pureColorCars.end()){
//                iter->second.emplace_back(i.carNo-1);
//            }
//            else{
//                pureColorCars.insert(pair<string, vector<int>>{i.bodyColor, vector<int>{i.carNo - 1}});
//            }
//        }
//        else{
//            mixedColorCars.emplace_back(i.carNo-1);
//        }
//    }
//
//    while(!mixedColorCars.empty()){
//        int index = ::rand() % mixedColorCars.size();           // 随机选择一个混色
//        chro.sequence.emplace_back(mixedColorCars[index]);
//
////        cout << mixedColorCars[index] << ' ' << ins.cars[mixedColorCars[index]].bodyColor << endl;
//        mixedColorCars.erase(mixedColorCars.begin() + index);
////        for(auto i: mixedColorCars){
////            cout << i << " ";
////        }cout << endl;
//
//        if(!pureColorCars.empty()){         // 选择与混色车身相同的颜色
//            unordered_map<string, vector<int>>::iterator iter;
//            if((iter = pureColorCars.find(ins.cars[mixedColorCars[index]].bodyColor)) != pureColorCars.end()){
//
//            }
//            else{
//                index = ::rand() % pureColorCars.size();    // 随机选择一个颜色
//                iter = pureColorCars.begin();
//                for(int i = 0; i != index; ++i){
//                    ++iter;
//                }
//            }
////            unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
////            shuffle(iter->second.begin(), iter->second.end(), default_random_engine(seed));
//
//            int size = iter->second.size();
//            for(int i = 0; i != min(5, size); ++i){         // TODO 可能空隙不到 同色/4 那要改为5的倍数
//                chro.sequence.emplace_back(iter->second[0]);
//                iter->second.erase(iter->second.begin());
//            }
//            if(iter->second.size() == 0)    pureColorCars.erase(iter);
//        }
//
//    }
//    for(auto a: chro.sequence){
//        cout << a << ' ';
//    }cout << endl;
//    evaluation(chro);
//    cout << chro.objs[0] << ' ' << chro.objs[1] << ' ' << chro.objs[2] << endl;
//    cout << "---------------" << endl;

}

void ImprovedNSGA2::cross(vector<chromosome>& population, int& iter) {
    for(int i = 0; i < population.size(); i += 2){
        vector<int> parent1 = population[i].sequence;
        vector<int> parent2 = population[i+1].sequence;

        if(parent1 == parent2){
           if(iter < maxIter / 3){
                chromosome child1 = chromosome{parent1, vector<double>(3, 0)};
                chromosome child2 = chromosome{parent2, vector<double>(3, 0)};
                evaluation(child1);         // 更新child1和child2参数
                evaluation(child2);
                population[i] = child1.objs[0] == std::numeric_limits<double>::infinity() ? population[i] : child1;          // 如果为可行解 则保留 否则保留原解
                population[i + 1] = child2.objs[0] == std::numeric_limits<double>::infinity() ? population[i + 1] : child2;
           }
           else{
               population[i].sequence = parent1;
               population[i + 1].sequence = parent2;
           }
           continue;
        }

        // 生成位置序列（0-1）
        vector<int> position(parent1.size(), 0);

        // 记录需要换顺序的元素
        vector<pair<int, int>> genes1 = {};
        vector<pair<int, int>> genes2 = {};
        for(int i = 0; i != position.size(); ++i){
            position[i] = ::rand() % 2;
            if(position[i] == 1){
                genes1.emplace_back(pair<int, int>{parent1[i], 0});
                genes2.emplace_back(pair<int, int>{parent2[i], 0});
            }
        }

//        for(auto i: genes1)  cout << "(" << i.first << ',' << i.second << ") ";cout << endl;
//        for(auto i: genes2)  cout << "(" << i.first << ',' << i.second << ") ";cout << endl;

        for(auto& i: genes1){        // 找到在parent2中的位置
            i.second = std::find(parent2.begin(), parent2.end(),i.first) - parent2.begin();
        }
        for(auto& i: genes2){        // 找到在parent1中的位置
            i.second = std::find(parent1.begin(), parent1.end(),i.first) - parent1.begin();
        }
        sort(genes1.begin(), genes1.end(), cmp1);
        sort(genes2.begin(), genes2.end(), cmp1);

//        for(auto i: genes1)  cout << "(" << i.first << ',' << i.second << ") ";cout << endl;
        for(int i = 0, j = 0; j != genes1.size(); ++i){
            if(position[i] == 1){
                parent1[i] = genes1[j].first;
                parent2[i] = genes2[j++].first;
            }
        }

        if(iter < maxIter / 3){
            chromosome child1 = chromosome{parent1, vector<double>(3, 0)};
            chromosome child2 = chromosome{parent2, vector<double>(3, 0)};
            evaluation(child1);         // 更新child1和child2参数
            evaluation(child2);
            population[i] = child1.objs[0] == std::numeric_limits<double>::infinity() ? population[i] : child1;          // 如果为可行解 则保留 否则保留原解
            population[i + 1] = child2.objs[0] == std::numeric_limits<double>::infinity() ? population[i + 1] : child2;
        }
        else{
            population[i].sequence = parent1;
            population[i + 1].sequence = parent2;
        }

//        if(population[i].objs[0] == INT_MAX / 2 or population[i+1].objs[0] == INT_MAX / 2)  goto label;     // 必须生成可行解

    }

}

void ImprovedNSGA2::mutation(vector<chromosome> &population, int& iter) {
    for(int i = 0; i < population.size(); ++i){
        vector<int> parent = population[i].sequence;
        // 生成位置序列（0-1）
        int oneNum = ::rand() % (parent.size() / 2);    // 变异位置数量
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
        chromosome child = chromosome{parent, vector<double>(3, 0)};
        evaluation(child);         // 更新child参数
        population[i] = child.objs[0] == std::numeric_limits<double>::infinity() ? population[i] : child;          // 如果为可行解 则保留 否则保留原解

    }
}

// void ImprovedNSGA2::nondominatedSorting(vector<chromosome> &population) {
//     // 非支配排序
//     for (int i = 0; i < populationSize; ++i) {
//         population[i].populationIndex = i;
//         population[i].crowding_distance = 0;
//         population[i].rank = 0;
//     }

//     vector<vector<chromosome>> fronts = {};
//     vector<chromosome> front = {};
//     vector<int> dominatingNum(populationSize, 0);
//     vector<vector<int>> dominatedSet(populationSize, vector<int>());
//     int k = 0;
//     for(int popIndex = 0; popIndex < populationSize; ++popIndex){
//         dominatingNum[popIndex] = 0;
//         for (int otherPopIndex = 0; otherPopIndex < populationSize; ++otherPopIndex) {
//             if (population[popIndex] < population[otherPopIndex]){
//                 dominatedSet[popIndex].emplace_back(otherPopIndex);
//             } else if (population[otherPopIndex] < population[popIndex]){
//                 dominatingNum[popIndex]++;
//             }
//         }

//         if(dominatingNum[popIndex] == 0){
//             population[popIndex].rank = 0;
//             front.emplace_back(population[popIndex]);
//         }
//     }
//     fronts.emplace_back(front);
//     int frontIndex = 0;
//     while(!fronts[frontIndex].empty()){
//         vector<chromosome> Q = {};
//         for(auto& pop: fronts[frontIndex]){
//             for(auto& otherPopIndex: dominatedSet[pop.populationIndex]){
//                 --dominatingNum[otherPopIndex];
//                 if(dominatingNum[otherPopIndex] == 0){
//                     population[otherPopIndex].rank = frontIndex + 1;
//                     Q.emplace_back(population[otherPopIndex]);
//                 }
//             }
//         }
//         ++frontIndex;
//         fronts.emplace_back(Q);
//     }
//     fronts.pop_back();

//     // 计算拥挤距离
//     int objNum = population[0].objs.size();
//     for (auto &front : fronts) {
//         for (int obj_i = 0; obj_i < objNum; ++obj_i) {
//             sort(front.begin(), front.end(), [&](const chromosome &a, const chromosome &b) {
//                 return a.objs[obj_i] < b.objs[obj_i];
//             });
//             double norm = (front[front.size() - 1].objs[obj_i] - front[0].objs[obj_i]);
//             if(norm != 0){
//                 front[0].crowding_distance = std::numeric_limits<double>::infinity();
//                 int frontSize = front.size();
//                 front[frontSize - 1].crowding_distance = std::numeric_limits<double>::infinity();
//                 for (int index = 1; index < frontSize - 1; ++index) {
//                     front[index].crowding_distance += (front[index + 1].objs[obj_i] - front[index - 1].objs[obj_i]) / norm;
//                 }
//             }
//         }
//     }


//     population = {};
//     for(auto& front: fronts){
//         sort(front.begin(), front.end(), [](const chromosome &a, const chromosome &b) {
//             return a.crowding_distance > b.crowding_distance;
//         });
//         for(auto& p: front){
//             population.emplace_back(p);
//         }
//     }
// }

void ImprovedNSGA2::nondominatedSorting(vector<chromosome> &population) {
    // 非支配排序
    const int populationSize = population.size();
    for (int i = 0; i < populationSize; ++i) {
        population[i].populationIndex = i;
        population[i].crowding_distance = 0;
        population[i].rank = 0;
    }

    vector<vector<chromosome>> fronts = {};
    vector<chromosome> front = {};
    vector<int> dominatingNum(populationSize, 0);
    vector<vector<int>> dominatedSet(populationSize, vector<int>());
    int k = 0;
    for(int popIndex = 0; popIndex < populationSize; ++popIndex){
        dominatingNum[popIndex] = 0;
        for (int otherPopIndex = 0; otherPopIndex < populationSize; ++otherPopIndex) {
            if (population[popIndex] < population[otherPopIndex]){
                dominatedSet[popIndex].emplace_back(otherPopIndex);
            } else if (population[otherPopIndex] < population[popIndex]){
                dominatingNum[popIndex]++;
            }
        }

        if(dominatingNum[popIndex] == 0){
            population[popIndex].rank = 0;
            front.emplace_back(population[popIndex]);
        }
    }
    fronts.emplace_back(front);
    int frontIndex = 0;
    while(!fronts[frontIndex].empty()){
        vector<chromosome> Q = {};
        for(auto& pop: fronts[frontIndex]){
            for(auto& otherPopIndex: dominatedSet[pop.populationIndex]){
                --dominatingNum[otherPopIndex];
                if(dominatingNum[otherPopIndex] == 0){
                    population[otherPopIndex].rank = frontIndex + 1;
                    Q.emplace_back(population[otherPopIndex]);
                }
            }
        }
        ++frontIndex;
        fronts.emplace_back(Q);
    }
    fronts.pop_back();

    computeCrowdingDistance(population, fronts);
}

void ImprovedNSGA2::computeCrowdingDistance(vector<chromosome> &population, vector<vector<chromosome>> &fronts) {
    // 计算拥挤距离
    const int populationSize = population.size();
    const int objNum = population[0].objs.size();
    const int needSize = populationSize / 2;
    int sortedSize = 0;
    for (auto &front : fronts) {
        sortedSize += front.size();
        if (sortedSize < needSize) {
            continue;
        }
        if (sortedSize == needSize) {
            break;
        }
        for (int obj_i = 0; obj_i < objNum; ++obj_i) {
            sort(front.begin(), front.end(), [&](const chromosome &a, const chromosome &b) {
                return a.objs[obj_i] < b.objs[obj_i];
            });
            double norm = (front[front.size() - 1].objs[obj_i] - front[0].objs[obj_i]);
            if(norm != 0){
                front[0].crowding_distance = std::numeric_limits<double>::infinity();
                int frontSize = front.size();
                front[frontSize - 1].crowding_distance = std::numeric_limits<double>::infinity();
                for (int index = 1; index < frontSize - 1; ++index) {
                    front[index].crowding_distance += (front[index + 1].objs[obj_i] - front[index - 1].objs[obj_i]) / norm;
                }
            }
        }
        break;
    }

    population = {};
    for(auto& front: fronts){
        sort(front.begin(), front.end(), [](const chromosome &a, const chromosome &b) {
            return a.crowding_distance > b.crowding_distance;
        });
        for(auto& p: front){
            population.emplace_back(p);
        }
    }
}

void ImprovedNSGA2::pretreatSpeedTrans(vector<int> &type2, vector<int> &type4) {
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
    sort(position.begin(), position.begin() + type4Set.size(), cmp);

    for(int i = 0; i != type4Set.size(); ++i){
        type2.insert(type2.begin() + position[i], type4Set[i].begin(), type4Set[i].end());
    }
}

bool ImprovedNSGA2::cmp(int a, int b){
    return a > b;
}

bool ImprovedNSGA2::cmp1(pair<int, int> a, pair<int, int> b){
    return a.second < b.second;
}