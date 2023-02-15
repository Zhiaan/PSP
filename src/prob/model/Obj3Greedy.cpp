//
// Created by Zhiaan on 2023/2/14.
//

#include "Obj3Greedy.h"
Obj3Greedy::Obj3Greedy(instance inst, vector<int> sequence) {
    ins = inst;
    neighborSize = 500;
    maxIterTime = 200;
    existSolution = {sequence, 0, 0, 0, 0};
}

vector<solution> Obj3Greedy::Obj3GreedyRunner() {
    vector<solution> solutions;

    sol globalBestSolution;
    sol localBestSolution;
    if(existSolution.sequence.size() == 0){
        localBestSolution = generateSolution();
        globalBestSolution = localBestSolution;
    }
    else{
        evaluation(existSolution);
        localBestSolution = existSolution;
        globalBestSolution = existSolution;
    }

    int flag = 0;
    while(true){
        printf("current threadId: %d, current instance: %s, flag: %d\n", ins.threadId, ins.instanceNo.c_str(), flag);
        sol bestNeighbor;
        bestNeighbor.obj3 = INT_MAX;
        for(int j = 0; j != neighborSize; ++j){
            sol neighbor = localBestSolution;

            mutation(neighbor);

//            swap1(neighbor);
//            particallySwapMutation(neighbor);
            if((neighbor.obj3 < bestNeighbor.obj3) \
 or (neighbor.obj3 == bestNeighbor.obj3 and neighbor.obj1 <= bestNeighbor.obj1 and
     ((neighbor.obj2 <= bestNeighbor.obj2 and neighbor.obj4 < bestNeighbor.obj4) or (neighbor.obj2 < bestNeighbor.obj2 and neighbor.obj4 <= bestNeighbor.obj4)))) bestNeighbor = neighbor;

//            cout << neighbor.obj4 << ' ' << bestNeighbor.obj4 << endl;
        }
        localBestSolution = bestNeighbor;
        if((bestNeighbor.obj3 < globalBestSolution.obj3) \
 or (bestNeighbor.obj3 == globalBestSolution.obj3 and bestNeighbor.obj1 <= globalBestSolution.obj1 and
     ((bestNeighbor.obj2 <= globalBestSolution.obj2 and bestNeighbor.obj4 < globalBestSolution.obj4) or (bestNeighbor.obj2 < globalBestSolution.obj2 and bestNeighbor.obj4 <= globalBestSolution.obj4)))) {
            globalBestSolution = bestNeighbor;
            flag = 0;
        }
        else{
            ++flag;
        }
//        cout << localBestSolution.obj2 << ' ' << globalBestSolution.obj2 << endl;
//        cout << "-----------------" << endl;
        if(flag == maxIterTime) break;
    }
    cout << ins.instanceNo << ' ' << globalBestSolution.obj1 << ' ' << globalBestSolution.obj2 << ' ' << globalBestSolution.obj3 << ' ' << globalBestSolution.obj4 << endl;


//    for(int i: localBestSolution.sequence){
//        cout << i << ' ';
//    }cout << endl;
//    cout << localBestSolution.obj1 << ' ' <<  localBestSolution.obj2 << ' ' << localBestSolution.obj4 << endl;
//    cout << "insNo:" << ins.instanceNo << "obj4: " << globalBestSolution.obj4 << endl;
    solution s;
    s.sequence = globalBestSolution.sequence;
    s.obj1 = globalBestSolution.obj1;
    s.obj2 = globalBestSolution.obj2;
    s.obj3 = globalBestSolution.obj3;
    s.obj4 = globalBestSolution.obj4;
    solutions.emplace_back(s);
    return solutions;
}

sol Obj3Greedy::generateSolution(){
    sol initSolution;
    auto sortedByType = [](vector<carInfo> &cars) {
        std::sort(cars.begin(), cars.end(), [](const carInfo &a, const carInfo &b) {
            return a.type < b.type;
        });
    };

    vector<carInfo> cars_type = ins.cars;
    sortedByType(cars_type);
    auto iter_type = std::find_if(cars_type.begin(), cars_type.end(), [](const carInfo &a) {
        return a.type == "B";
    });
    vector<carInfo> cars_A = vector<carInfo>(cars_type.begin(), iter_type);
    vector<carInfo> cars_B = vector<carInfo>(iter_type, cars_type.end());

    vector<carInfo> carsA2, carsA4, carsB2, carsB4;

    for(auto car: cars_A){
        if(car.speedTrans == "两驱"){
            carsA2.emplace_back(car);
        }
        if(car.speedTrans == "四驱"){
            carsA4.emplace_back(car);
        }
    }
    for(auto car: cars_B){
        if(car.speedTrans == "两驱"){
            carsB2.emplace_back(car);
        }
        if(car.speedTrans == "四驱"){
            carsB4.emplace_back(car);
        }
    }

    auto seed = std::chrono::system_clock::now().time_since_epoch().count();
    shuffle (carsA2.begin(), carsA2.end(), std::default_random_engine(seed));
    seed = std::chrono::system_clock::now().time_since_epoch().count();
    shuffle (carsA4.begin(), carsA4.end(), std::default_random_engine(seed));
    seed = std::chrono::system_clock::now().time_since_epoch().count();
    shuffle (carsB2.begin(), carsB2.end(), std::default_random_engine(seed));
    seed = std::chrono::system_clock::now().time_since_epoch().count();
    shuffle (carsB4.begin(), carsB4.end(), std::default_random_engine(seed));

    cars_A = carsA2;
    cars_B = carsB2;

    vector<vector<carInfo>> carsA4Split = splitVector(carsA4);
    vector<vector<carInfo>> carsB4Split = splitVector(carsB4);

    vector<int> positionsA, positionsB;
    vector<int> randPosition;
    for(int i = 0; i != cars_A.size(); ++i){
        randPosition.emplace_back(i);
    }
    seed = std::chrono::system_clock::now().time_since_epoch().count();
    shuffle (randPosition.begin(), randPosition.end(), std::default_random_engine(seed));

    positionsA.insert(positionsA.begin(), randPosition.begin(), randPosition.begin() + carsA4Split.size());

    randPosition.clear();
    for(int i = 0; i != cars_B.size(); ++i){
        randPosition.emplace_back(i);
    }
    seed = std::chrono::system_clock::now().time_since_epoch().count();
    shuffle (randPosition.begin(), randPosition.end(), std::default_random_engine(seed));

    positionsB.insert(positionsB.begin(), randPosition.begin(), randPosition.begin() + carsA4Split.size());


    sort(positionsA.begin(), positionsA.end(), greater<int>());
    sort(positionsB.begin(), positionsB.end(), greater<int>());


    for(int i = 0; i != carsA4Split.size(); ++i){
        cars_A.insert(cars_A.begin() + positionsA[i], carsA4Split[i].begin(), carsA4Split[i].end());
    }
    for(int i = 0; i != carsB4Split.size(); ++i){
        cars_B.insert(cars_B.begin() + positionsB[i], carsB4Split[i].begin(), carsB4Split[i].end());
    }

    int randNum = ::rand() % 2;
    if(randNum == 0){
        for(auto i: cars_A)     initSolution.sequence.emplace_back(i.carNo);
        for(auto i: cars_B)     initSolution.sequence.emplace_back(i.carNo);
    }
    else{
        for(auto i: cars_B)     initSolution.sequence.emplace_back(i.carNo);
        for(auto i: cars_A)     initSolution.sequence.emplace_back(i.carNo);
    }
//    for(auto i: carsB4){
//        cout << i.carNo << ' ';
//    }cout << endl;
//
//    for(auto i: carsB4Split){
//        cout << "[";
//        for(auto j: i){
//            cout << j.carNo << ' ';
//        }cout << ']';
//    }cout << endl;

//    cout << initSolution.sequence.size() << endl;
    evaluation(initSolution);
    return initSolution;

}

vector<vector<carInfo>> Obj3Greedy::splitVector(vector<carInfo> cars4) {
    int index = 0;

    vector<vector<carInfo>> cars4split;

    while(index < cars4.size()){
        int length = ::rand() % 2 + 2;
        if(index == cars4.size() - 4){
            length = 2;
        }
        else if(index == cars4.size() - 3){
            length = 3;
        }
        else if(index == cars4.size() - 2){
            length = 2;
        }
        else if(index == cars4.size() - 1){
            length = 1;
        }
        else if(index == cars4.size() - 0){
            length = 0;
        }
        int flag = index + length;
        vector<carInfo> split;
        for(; index != flag; ++index){
            split.emplace_back(cars4[index]);
        }
        cars4split.emplace_back(split);
    }

    return cars4split;
}

void Obj3Greedy::evaluation(sol &c) {
    c.obj2 = 0;
    c.obj1 = 0;  // obj0和1目标值归零
    c.obj3 = 0;
    c.obj4 = 0;
    vector<int> test = c.sequence;
    sort(test.begin(), test.end());
    vector<int> examine(ins.cars.size());
    for(int i = 0; i != examine.size(); ++i){
        examine[i] = i;
    }
    if(examine != test){
        c.obj1 = c.obj2 = c.obj3  = c.obj4 = -1;
        cout << "error" << endl;
        return;
    }

    int typeCommonTime = 1;
    int colorCommonTime = 0;
    int speedTransCommonTime = 0;   // 连续四驱次数
    int obj2Cost = 0;   // obj2惩罚值，限制偏向颜色以 5 的倍数切换生产
    for(int i = 0; i != c.sequence.size() - 1; ++i){
        // obj3 记录超出4辆连续四驱的惩罚洗漱 以及 一辆四驱的惩罚系数
        if(ins.cars[c.sequence[i]].speedTrans == "四驱"){
            ++speedTransCommonTime;
            if(speedTransCommonTime >= 4){  // 连续四驱数量到4 记录超越次数作为惩罚值obj3
                c.obj3 += 2;
            }
        }
        else{
            if(speedTransCommonTime == 1)   c.obj3 += 1;
            speedTransCommonTime = 0;
        }

        // obj2 焊装总切换次数 记录因为不同类型导致的切换次数
        if(ins.cars[c.sequence[i]].type == ins.cars[c.sequence[i + 1]].type){   // 如果前后相等 记录连续相等车数
            ++typeCommonTime;
        }
        else{
            ++c.obj1;     // 如果前后不相等 记录切换次数
            if(typeCommonTime * ins.weldingTime < ins.weldingContinueTime){     // 如果前后不相等且小于30min 总时长增加
                c.obj4 += ins.weldingContinueTime - typeCommonTime * ins.weldingTime;
            }
            typeCommonTime = 1;
        }

        // obj3 记录未达到连续五辆同色车的惩罚系数
        if (ins.cars[c.sequence[i]].checkRoofNotEqualBody()) { // 本车车顶颜色!=车身颜色
            ++c.obj2;
            if (colorCommonTime != 0) {
                obj2Cost += (5 - colorCommonTime);
            }
            colorCommonTime = 0;
        }
        else{               // 本车车顶颜色=车身颜色
            ++colorCommonTime;
            if(colorCommonTime == 5){       // 如果五辆车颜色相同
                ++c.obj2;
                colorCommonTime = 0;
                continue;
            }

        }
        // 如果前车车身!=后车车顶
        if (ins.cars[c.sequence[i]].checkBodyNotEqualNextRoof(ins.cars[c.sequence[i + 1]])) {
            ++c.obj2;
            if (colorCommonTime != 0) {
                obj2Cost += (5 - colorCommonTime);
            }
            colorCommonTime = 0;
        }
    }

    if(typeCommonTime * ins.weldingTime < ins.weldingContinueTime){     // 如果前后不相等且小于30min 总时长增加
        c.obj4 += ins.weldingContinueTime - typeCommonTime * ins.weldingTime;
    }
    if(speedTransCommonTime == 3 and ins.cars[c.sequence.back()].speedTrans == "四驱"){
        c.obj3 += 2;     // 如果前后不相等 记录总装车间切换次数
    }
    else if(speedTransCommonTime == 0 and ins.cars[c.sequence.back()].speedTrans == "四驱"){
        c.obj3 += 1;
    }
    else if(speedTransCommonTime == 1 and ins.cars[c.sequence.back()].speedTrans == "两驱"){
        c.obj3 += 1;
    }

    if (ins.cars[c.sequence.back()].checkRoofNotEqualBody()) {
        if (colorCommonTime != 0) {
            obj2Cost += (5 - colorCommonTime);
        }
        ++c.obj2;
    }
    else {
        // 车身车顶颜色相同
        ++colorCommonTime;
        obj2Cost += (5 - colorCommonTime);
    }

    // obj4
    c.obj4 += ins.weldingTime * c.sequence.size() + c.obj2 * ins.paintingWaitingTime + ins.paintingTime * 2 * c.sequence.size() + ins.assembleTime * c.sequence.size();
    c.obj2 = obj2Cost;
}

void Obj3Greedy::mutation(sol& solution){
    int randPosition1 = ::rand() % solution.sequence.size();
    int randPosition2 = ::rand() % solution.sequence.size();
//    cout << randPosition1 << ' ' << randPosition2 << endl;
//    cout << ins.cars[solution.sequence[randPosition1]].speedTrans << ' ' << ins.cars[solution.sequence[randPosition2]].speedTrans << endl;
    if(ins.cars[solution.sequence[randPosition1]].speedTrans == "四驱" and ins.cars[solution.sequence[randPosition2]].speedTrans == "四驱" or
    ins.cars[solution.sequence[randPosition1]].speedTrans == "两驱" and ins.cars[solution.sequence[randPosition2]].speedTrans == "两驱"){
        swap(solution.sequence[randPosition1], solution.sequence[randPosition2]);

    }
    else{
        if(ins.cars[solution.sequence[randPosition1]].speedTrans == "两驱"){
            swap(randPosition1, randPosition2);             // position1：四驱 position2：两驱
        }
        while(randPosition2 == 0 and ins.cars[solution.sequence[randPosition2 + 1]].speedTrans == "四驱" or
        randPosition2 == solution.sequence.size() - 1 and ins.cars[solution.sequence[randPosition2 - 1]].speedTrans == "四驱" or
        ins.cars[solution.sequence[randPosition2 + 1]].speedTrans == "四驱" or ins.cars[solution.sequence[randPosition2 - 1]].speedTrans == "四驱"){
            randPosition2 = ::rand() % solution.sequence.size();
        }

        int pre = randPosition1, behind = randPosition1;        // 四驱片段[pre, behind)
        while(ins.cars[solution.sequence[pre]].speedTrans == "四驱" and pre >= 0){
            --pre;
        }
        ++pre;
        while(ins.cars[solution.sequence[behind]].speedTrans == "四驱" and behind < solution.sequence.size()){
            ++behind;
        }

        vector<int> newSequence;
        if(randPosition2 < pre){
            newSequence.insert(newSequence.end(), solution.sequence.begin(), solution.sequence.begin() + randPosition2);
            newSequence.insert(newSequence.end(), solution.sequence.begin() + pre, solution.sequence.begin() + behind);
            newSequence.insert(newSequence.end(), solution.sequence.begin() + randPosition2 + 1, solution.sequence.begin() + pre);
            newSequence.insert(newSequence.end(), solution.sequence.begin() + randPosition2, solution.sequence.begin() + randPosition2 + 1);
            newSequence.insert(newSequence.end(), solution.sequence.begin() + behind, solution.sequence.end());
        }
        if(randPosition2 > behind){
            newSequence.insert(newSequence.end(), solution.sequence.begin(), solution.sequence.begin() + pre);
            newSequence.insert(newSequence.end(), solution.sequence.begin() + randPosition2, solution.sequence.begin() + randPosition2 + 1);
            newSequence.insert(newSequence.end(), solution.sequence.begin() + behind, solution.sequence.begin() + randPosition2);
            newSequence.insert(newSequence.end(), solution.sequence.begin() + pre, solution.sequence.begin() + behind);
            newSequence.insert(newSequence.end(), solution.sequence.begin() + randPosition2 + 1, solution.sequence.end());
        }
        solution.sequence = newSequence;
    }

//    for(auto i: solution.sequence){
//        cout << i << ' ';
//    }cout << endl;

    evaluation(solution);
}