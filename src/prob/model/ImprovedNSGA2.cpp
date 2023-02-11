//
// Created by Zhiaan on 2023/1/12.
//

#include "ImprovedNSGA2.h"
ImprovedNSGA2::ImprovedNSGA2(instance inst) {
    ins = inst;
    populationSize = 600;
    chromosomeLength = ins.cars.size();
    crossTime = 5;
    maxIter = 1000;
    iter = 0;
}

vector<solution> ImprovedNSGA2::NSGA2Runner() {
    vector<solution> solutions;

    vector<chromosome> population(populationSize);

    // 生成初始解
    greedySortInitializePopulation(population);
//     greedyObj1InitializePopulation(population);     // obj2贪婪算法初始化种群
//     randomInitializePopulation(population);           // 随机初始化种群

    for(; iter < maxIter; ++iter){

        if(iter == maxIter / 2){
            for(auto& chro: population){
                evaluation(chro);
            }
        }
        printf("current threadId: %d, current instance: %s, current iter: %d\n", ins.threadId, ins.instanceNo.c_str(), iter);
//        int num = 0;
//        for(auto i: population){
//            if(i.rank != 0) break;
//            ++num;
//            for(auto j: i.objs){
//                cout << j << ' ';
//            }cout << i.rank << ' ' << i.crowding_distance << endl;
//        }
//        cout << num << endl;
        vector<chromosome> newPopulation = population;      // 生成新种群

//        cross(newPopulation);                           // 交叉算子
        colorCommonCross(newPopulation);           // 颜色相同交叉
//        particallyMappedCross(newPopulation);       // PMX

        // mutation(newPopulation);                        // 变异算子
        particallySwapMutation(newPopulation);                        // 变异算子

        newPopulation.insert(newPopulation.end(), population.begin(), population.end());    // 老种群与新种群合并

        nondominatedSorting(newPopulation);     // 非支配排序
        population.clear();
        population.insert(population.begin(), newPopulation.begin(), newPopulation.begin() + populationSize);

    }

    for(auto i: population) {
        if(i.rank != 0 or solutions.size() == 50) break;
        solution s;
        s.sequence = i.sequence;
        s.obj1 = i.objs[0];
        s.obj2 = i.objs[1];
        s.obj3 = i.objs[2];
        s.obj4 = i.objs[3];
        solutions.emplace_back(s);
        cout << i.objs[0] << ' ' << i.objs[1] << ' ' << i.objs[2] << ' ' << (i.objs[2] >> 41) << ' ' << i.objs[3] << ' ' << i.rank  << ' ' << i.crowding_distance  << endl;

        // for (auto j: i.sequence) {
        //     cout << j << ' ';
        // } cout << endl;
    }

    return solutions;
}

// 二进制格式表示如下
// 0-10 位二进制数表示连续次数为2的数量，范围为 0-2047
// 11-20 位二进制数表示连续次数为3的数量，范围为 0-1023
// 21-30 位二进制数表示连续次数为4的数量，范围为 0-1023
// 31-40 位二进制数表示连续次数为5的数量，范围为 0-1023
// 41-54 位二进制数表示惩罚值，范围为 0~16384，但是惩罚值越小越好。
// 需要共计 55 位数表示
// 取负数可以用 (~num) + 1
const int binary[5] = {
    0,
    11,
    21,
    31,
    41,
};

long long computeObj2(vector<long long> &obj2Count) {
    long long obj2 = 0;
    for (int i = 0; i < obj2Count.size() - 1; i++) {
        obj2 += (obj2Count[i] << binary[i]);
    }
    obj2 = ((obj2Count.back() + 1) << binary[4]) - obj2;
    return obj2;
}

void ImprovedNSGA2::evaluation(chromosome &c) {
    c.objs = vector<long long>(4, 0);  // 四目标值归零
    vector<int> test = c.sequence;
    sort(test.begin(), test.end());
    vector<int> examine(ins.cars.size());
    for(int i = 0; i != examine.size(); ++i){
        examine[i] = i;
    }
    if(examine != test){
        c.objs[0] = c.objs[1] = c.objs[2]  = c.objs[3] = -1;
        cout << "sequence error" << endl;
        cout << ins.instanceNo << ' ' << iter << endl;
        cout << examine.size() << ' ' << test.size() << endl;
        return;
    }

    int typeCommonTime = 1;
    int colorCommonTime = 0;
    int speedTransCommonTime = 0;   // 连续四驱次数
    vector<long long> obj2Count(5);       // 相同颜色统计值{0:连续为2的个数 1:连续为3的个数 2:连续为4的个数 3:连续为5的个数 惩罚值}
    long long& obj2Cost = obj2Count[4];

    for(int i = 0; i != c.sequence.size() - 1; ++i){
        // obj3 记录超出4辆连续四驱的惩罚洗漱 以及 一辆四驱的惩罚系数
        if(ins.cars[c.sequence[i]].speedTrans == "四驱"){
            ++speedTransCommonTime;
            if(speedTransCommonTime >= 4){  // 连续四驱数量到4 记录超越次数作为惩罚值obj3
                c.objs[2] += 2;
            }
        }
        else{
            if(speedTransCommonTime == 1)   c.objs[2] += 1;
            speedTransCommonTime = 0;
        }

        // obj1 焊装总切换次数 记录因为不同类型导致的切换次数
        if(ins.cars[c.sequence[i]].type == ins.cars[c.sequence[i + 1]].type){   // 如果前后相等 记录连续相等车数
            ++typeCommonTime;
        }
        else{
            ++c.objs[0];     // 如果前后不相等 记录切换次数
            if(typeCommonTime * ins.weldingTime < ins.weldingContinueTime){     // 如果前后不相等且小于30min 总时长增加
                c.objs[3] += ins.weldingContinueTime - typeCommonTime * ins.weldingTime;
            }
            typeCommonTime = 1;
        }

        // obj2 记录未达到连续五辆同色车的惩罚系数
        if (ins.cars[c.sequence[i]].checkRoofNotEqualBody()) { // 本车车顶颜色!=车身颜色
            ++c.objs[1];
            if (colorCommonTime != 0) {
                obj2Cost += (5 - colorCommonTime);
                ++obj2Count[colorCommonTime - 2];
            }
            colorCommonTime = 0;
        }
        else{               // 本车车顶颜色=车身颜色
            ++colorCommonTime;
            if(colorCommonTime == 5){       // 如果五辆车颜色相同
                ++c.objs[1];
                ++obj2Count[colorCommonTime - 2];
                colorCommonTime = 0;
                continue;
            }

        }
        // 如果前车车身!=后车车顶
        if (ins.cars[c.sequence[i]].checkBodyNotEqualNextRoof(ins.cars[c.sequence[i + 1]])) {
            ++c.objs[1];
            if (colorCommonTime != 0) {
                obj2Cost += (5 - colorCommonTime);
                ++obj2Count[colorCommonTime - 2];
            }
            colorCommonTime = 0;
        }

    }
    if(speedTransCommonTime >= 3 and ins.cars[c.sequence.back()].speedTrans == "四驱"){
        c.objs[2] += 2;     // 如果前后不相等 记录总装车间切换次数
    }
    else if(speedTransCommonTime == 0 and ins.cars[c.sequence.back()].speedTrans == "四驱"){
        c.objs[2] += 1;
    }
    else if(speedTransCommonTime == 1 and ins.cars[c.sequence.back()].speedTrans == "两驱"){
        c.objs[2] += 1;
    }


    if (ins.cars[c.sequence.back()].checkRoofNotEqualBody()) {
        ++c.objs[1];
        if (colorCommonTime != 0) {
            obj2Cost += (5 - colorCommonTime);
            ++obj2Count[colorCommonTime - 2];
        }
    }
    else {
        // 车身车顶颜色相同
        ++colorCommonTime;
        if(colorCommonTime != 1){
            ++obj2Count[colorCommonTime - 2];
        }
        obj2Cost += (5 - colorCommonTime);
    }

    // obj4
    c.objs[3] += ins.weldingTime * c.sequence.size() + c.objs[1] * ins.paintingWaitingTime + ins.paintingTime * 2 * c.sequence.size() + ins.assembleTime * c.sequence.size();
//    if(iter >= maxIter / 2){
    c.objs[1] = computeObj2(obj2Count);
//    }


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
        chro.objs = vector<long long>(4, 0);
        evaluation(chro);
//        if(chro.objs[0] == std::numeric_limits<double>::infinity())  goto label;     // 生成可行解 不可行解重新生成

    }
}

void ImprovedNSGA2::greedySortInitializePopulation(vector<chromosome>& population) {
    auto sortedByType = [](vector<carInfo> &cars) {
        std::sort(cars.begin(), cars.end(), [](const carInfo &a, const carInfo &b) {
            return a.type < b.type;
        });
    };
    auto splitColor = [](vector<carInfo> &cars, vector<carInfo> &saved_same_color_cars, vector<carInfo> &saved_not_same_color_cars) {
        // 划分车身车顶颜色相同和不同的车
        for (auto car: cars) {
            if(car.roofColor != "无对比颜色" and car.roofColor != car.bodyColor){     // 本车车顶颜色!=车身颜色
                saved_not_same_color_cars.emplace_back(car);
            } else{
                saved_same_color_cars.emplace_back(car);
            }
        }
    };
    auto sortedByColor = [](vector<carInfo> &cars_same_color) {
        std::sort(cars_same_color.begin(), cars_same_color.end(), [](const carInfo &a, const carInfo &b) {
            return a.bodyColor < b.bodyColor;
        });
    };
    auto to_2d = [](vector<carInfo> &cars_same_color) -> vector<vector<carInfo>> {
        vector<vector<carInfo>> saved_cars_same_color;
        vector<carInfo> temp;
        int i;
        for (i = 0; i < cars_same_color.size() - 1; ++i) {
            temp.emplace_back(cars_same_color[i]);
            if (cars_same_color[i].bodyColor != cars_same_color[i + 1].bodyColor) {
                saved_cars_same_color.emplace_back(temp);
                temp.clear();
            }
        }
        temp.emplace_back(cars_same_color[i]);
        saved_cars_same_color.emplace_back(temp);
        return saved_cars_same_color;
    };

    auto emplace_helper = [](chromosome &chro, vector<vector<carInfo>> &A_same_copy, vector<carInfo> &A_not_same_copy, vector<vector<carInfo>> &B_same_copy, vector<carInfo> &B_not_same_copy) {
        // 此函数保证 A 放前面，B 放后面
        // 为了减少代码冗余，当要 B 放前面时，只需相反顺序传参即可
        int rand_num = ::rand() % 4;
        if (rand_num == 0) {
            // A 相同颜色放前面，B 相同颜色放前面
            for (auto &cars_same_color: A_same_copy) {
                for (auto &car: cars_same_color) {
                    chro.sequence.emplace_back(car.carNo);
                }
            }
            for (auto &car: A_not_same_copy) {
                chro.sequence.emplace_back(car.carNo);
            }
            for (auto &cars_same_color: B_same_copy) {
                for (auto &car: cars_same_color) {
                    chro.sequence.emplace_back(car.carNo);
                }
            }
            for (auto &car: B_not_same_copy) {
                chro.sequence.emplace_back(car.carNo);
            }
        } else if (rand_num == 1) {
            // A 相同颜色放前面，B 相同颜色后面
            for (auto &cars_same_color: A_same_copy) {
                for (auto &car: cars_same_color) {
                    chro.sequence.emplace_back(car.carNo);
                }
            }
            for (auto &car: A_not_same_copy) {
                chro.sequence.emplace_back(car.carNo);
            }
            for (auto &car: B_not_same_copy) {
                chro.sequence.emplace_back(car.carNo);
            }
            for (auto &cars_same_color: B_same_copy) {
                for (auto &car: cars_same_color) {
                    chro.sequence.emplace_back(car.carNo);
                }
            }
        } else if (rand_num == 2) {
            // A 相同颜色放后面，B 相同颜色放前面
            for (auto &car: A_not_same_copy) {
                chro.sequence.emplace_back(car.carNo);
            }
            for (auto &cars_same_color: A_same_copy) {
                for (auto &car: cars_same_color) {
                    chro.sequence.emplace_back(car.carNo);
                }
            }
            for (auto &cars_same_color: B_same_copy) {
                for (auto &car: cars_same_color) {
                    chro.sequence.emplace_back(car.carNo);
                }
            }
            for (auto &car: B_not_same_copy) {
                chro.sequence.emplace_back(car.carNo);
            }
        } else if (rand_num == 3) {
            // A 相同颜色放后面，B 相同颜色放后面
            for (auto &car: A_not_same_copy) {
                chro.sequence.emplace_back(car.carNo);
            }
            for (auto &cars_same_color: A_same_copy) {
                for (auto &car: cars_same_color) {
                    chro.sequence.emplace_back(car.carNo);
                }
            }
            for (auto &car: B_not_same_copy) {
                chro.sequence.emplace_back(car.carNo);
            }
            for (auto &cars_same_color: B_same_copy) {
                for (auto &car: cars_same_color) {
                    chro.sequence.emplace_back(car.carNo);
                }
            }
        } else {
            cout << "init error" << endl;
            assert(false);
        }
    };

    vector<carInfo> cars_type = ins.cars;
    sortedByType(cars_type);
    auto iter_type = std::find_if(cars_type.begin(), cars_type.end(), [](const carInfo &a) {
        return a.type == "B";
    });
    vector<carInfo> cars_A = vector<carInfo>(cars_type.begin(), iter_type);
    vector<carInfo> cars_B = vector<carInfo>(iter_type, cars_type.end());
    vector<carInfo> cars_A_same_color; 
    vector<carInfo> cars_A_not_same_color; // not split more, final use!!!
    vector<carInfo> cars_B_same_color;
    vector<carInfo> cars_B_not_same_color; // not split more, final use!!!
    splitColor(cars_A, cars_A_same_color, cars_A_not_same_color);
    splitColor(cars_B, cars_B_same_color, cars_B_not_same_color);
    sortedByColor(cars_A_same_color);
    sortedByColor(cars_B_same_color);

    vector<vector<carInfo>> split_cars_A_same_color = to_2d(cars_A_same_color); // final use !!!
    vector<vector<carInfo>> split_cars_B_same_color = to_2d(cars_B_same_color); // final use !!!

    int flag = 0;
    for (auto& chro: population) {
        if(flag++ == 0){
            string filePath1 = "../result/Obj1Greedy/" + ins.instanceNo + ".csv";
            IO io;
            vector<vector<string>> obj1GreedySolution = io.readCSV(filePath1);
            for(int j = 0; j != chromosomeLength; ++j){
                chro.sequence.emplace_back(stoi(obj1GreedySolution[0][j]));
            }
            evaluation(chro);
        }
        else{
            vector<carInfo> A_not_same_copy = cars_A_not_same_color; // not split more, final use!!!
            vector<carInfo> B_not_same_copy = cars_B_not_same_color; // not split more, final use!!!
            vector<vector<carInfo>> A_same_copy = split_cars_A_same_color; // final use !!!
            vector<vector<carInfo>> B_same_copy = split_cars_B_same_color; // final use !!!


            unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
            shuffle (A_not_same_copy.begin(), A_not_same_copy.end(), std::default_random_engine(seed));

            seed = std::chrono::system_clock::now().time_since_epoch().count();
            shuffle (B_not_same_copy.begin(), B_not_same_copy.end(), std::default_random_engine(seed));

            seed = std::chrono::system_clock::now().time_since_epoch().count();
            shuffle (A_same_copy.begin(), A_same_copy.end(), std::default_random_engine(seed));
            for (auto &vec : A_same_copy) {
                seed = std::chrono::system_clock::now().time_since_epoch().count();
                shuffle (vec.begin(), vec.end(), std::default_random_engine(seed));
            }

            seed = std::chrono::system_clock::now().time_since_epoch().count();
            shuffle (B_same_copy.begin(), B_same_copy.end(), std::default_random_engine(seed));
            for (auto &vec : B_same_copy) {
                seed = std::chrono::system_clock::now().time_since_epoch().count();
                shuffle (vec.begin(), vec.end(), std::default_random_engine(seed));
            }

            if (::rand() % 2) {
                // A 放前面
                emplace_helper(chro, A_same_copy, A_not_same_copy, B_same_copy, B_not_same_copy);
            } else {
                // B 放前面
                emplace_helper(chro, B_same_copy, B_not_same_copy, A_same_copy, A_not_same_copy);
            }
        }

    }

    for (auto &p: population) {
        evaluation(p);
        // for (auto &i : p.sequence) {
        //     auto iter = find_if(ins.cars.begin(), ins.cars.end(), [&](const carInfo &a) {
        //         return a.carNo == i;
        //     });
        //     cout << i << " " << iter->type << " " << iter->bodyColor << " " << iter->roofColor << endl;
        // }
        // cout << endl;
        // assert(false);
    }
}

void ImprovedNSGA2::greedyObj1InitializePopulation(vector<chromosome> &population){
    string filePath1 = "../result/Obj1Greedy/" + ins.instanceNo + ".csv";
    string filePath2 = "../result/Obj2Greedy/" + ins.instanceNo + ".csv";
    IO io;
    vector<vector<string>> obj1GreedySolution = io.readCSV(filePath1);
//    vector<vector<string>> obj2GreedySolution = io.readCSV(filePath2);
    vector<vector<string>> obj2GreedySolution = {};
    for(int i = 0; i != populationSize; ++i){
        if(i == 0){
            for(int j = 0; j != chromosomeLength; ++j){
                population[i].sequence.emplace_back(stoi(obj1GreedySolution[0][j]));
            }
            evaluation(population[i]);
        }
        else if(i < obj2GreedySolution.size() + 1){
            for(int j = 0; j != chromosomeLength; ++j){
                population[i].sequence.emplace_back(stoi(obj2GreedySolution[i-1][j]));
            }
            evaluation(population[i]);
        }
        else{
//            population[i] = population[0];
//            continue;
            chromosome& chro = population[i];
//            label:
//            chro.sequence = vector<int>(chromosomeLength, 0);
//            for(int index = 0; index != chromosomeLength; ++index){
//                chro.sequence[index] = index;
//            }
//            // 随机生成初序列
//            unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
//            shuffle (chro.sequence.begin(), chro.sequence.end(), std::default_random_engine(seed));
//
//            // 计算目标值
//            chro.objs = vector<double>(4, 0);
//            evaluation(chro);
//            if(chro.objs[0] == std::numeric_limits<double>::infinity())  goto label;     // 生成可行解 不可行解重新生成

//            unordered_map<string, vector<int>> pureColorCars;
//            vector<int> mixedColorCars = {};
//            for(auto i: ins.cars){
//                if(i.roofColor == i.bodyColor or i.roofColor == "无对比颜色"){
//                    unordered_map<string, vector<int>>::iterator iter;
//                    if((iter = pureColorCars.find(i.bodyColor)) != pureColorCars.end()){
//                        iter->second.emplace_back(i.carNo);
//                    }
//                    else{
//                        pureColorCars.insert(pair<string, vector<int>>{i.bodyColor, vector<int>{i.carNo}});
//                    }
//                }
//                else{
//                    mixedColorCars.emplace_back(i.carNo);
//                }
//            }
//
//            while(!mixedColorCars.empty()){
//                int index = ::rand() % mixedColorCars.size();           // 随机选择一个混色
//                chro.sequence.emplace_back(mixedColorCars[index]);
//
////        cout << mixedColorCars[index] << ' ' << ins.cars[mixedColorCars[index]].bodyColor << endl;
//                mixedColorCars.erase(mixedColorCars.begin() + index);
////        for(auto i: mixedColorCars){
////            cout << i << " ";
////        }cout << endl;
//
//                if(!pureColorCars.empty()){         // 选择与混色车身相同的颜色
//                    unordered_map<string, vector<int>>::iterator iter;
//                    if((iter = pureColorCars.find(ins.cars[mixedColorCars[index]].bodyColor)) != pureColorCars.end()){
//
//                    }
//                    else{
//                        index = ::rand() % pureColorCars.size();    // 随机选择一个颜色
//                        iter = pureColorCars.begin();
//                        for(int i = 0; i != index; ++i){
//                            ++iter;
//                        }
//                    }
////            unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
////            shuffle(iter->second.begin(), iter->second.end(), default_random_engine(seed));
//
//                    int size = iter->second.size();
//                    for(int i = 0; i != min(5, size); ++i){         // TODO 可能空隙不到 同色/4 那要改为5的倍数
//                        chro.sequence.emplace_back(iter->second[0]);
//                        iter->second.erase(iter->second.begin());
//                    }
//                    if(iter->second.size() == 0)    pureColorCars.erase(iter);
//                }
//
//            }
//
//            evaluation(chro);

        chro.sequence.clear();
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
            typeChoose = 1;      // 选择加入哪个序列
            if (typeChoose == 0 and behindA == typeA.size()) typeChoose = 1;
            if (typeChoose == 1 and behindB == typeB.size()) typeChoose = 0;

            switch (typeChoose) {
                case 0: {        // 选择typeA序列
                    if (typeA.size() - preA < 2 * 23) {
                        chro.sequence.insert(chro.sequence.end(), typeA.begin() + preA, typeA.end());
                        behindA = typeA.size();
                    } else {
                        int freeLength = remainA == 0 ? 0 : ::rand() % remainA;
                        behindA += 23 + freeLength;
                        chro.sequence.insert(chro.sequence.end(), typeA.begin() + preA, typeA.begin() + behindA);
                        remainA -= freeLength;
                        preA = behindA;
                    }

                    break;
                }
                case 1: {
                    if (typeB.size() - preB < 2 * 23) {
                        chro.sequence.insert(chro.sequence.end(), typeB.begin() + preB, typeB.end());
                        behindB = typeB.size();
                    } else {
                        int freeLength = remainB == 0 ? 0 : ::rand() % remainB;

                        //                    cout << freeLength << " B" << " ";
                        behindB += 23 + freeLength;
                        chro.sequence.insert(chro.sequence.end(), typeB.begin() + preB, typeB.begin() + behindB);
                        remainB -= freeLength;

                        //                    cout << remainA << " rB" << endl;
                        preB = behindB;
                    }
                    break;
                }
            }
        }
        evaluation(chro);
        }
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
//                    typeA.emplace_back(i.carNo);
//                } else if (i.type == "A" and i.speedTrans == "四驱") {
//                    typeA4.emplace_back(i.carNo);
//                } else if (i.type == "B" and i.speedTrans == "两驱") {
//                    typeB.emplace_back(i.carNo;
//                } else if (i.type == "B" and i.speedTrans == "四驱") {
//                    typeB4.emplace_back(i.carNo);
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

void ImprovedNSGA2::greedyObj2InitializePopulation(vector<chromosome> &population){
    chromosome chro;
    unordered_map<string, vector<int>> pureColorCars;
    vector<int> mixedColorCars = {};
    for(auto i: ins.cars){
        if(i.roofColor == i.bodyColor or i.roofColor == "无对比颜色"){
            unordered_map<string, vector<int>>::iterator iter;
            if((iter = pureColorCars.find(i.bodyColor)) != pureColorCars.end()){
                iter->second.emplace_back(i.carNo);
            }
            else{
                pureColorCars.insert(pair<string, vector<int>>{i.bodyColor, vector<int>{i.carNo}});
            }
        }
        else{
            mixedColorCars.emplace_back(i.carNo);
        }
    }

    while(!mixedColorCars.empty()){
        int index = ::rand() % mixedColorCars.size();           // 随机选择一个混色
        chro.sequence.emplace_back(mixedColorCars[index]);

//        cout << mixedColorCars[index] << ' ' << ins.cars[mixedColorCars[index]].bodyColor << endl;
        mixedColorCars.erase(mixedColorCars.begin() + index);
//        for(auto i: mixedColorCars){
//            cout << i << " ";
//        }cout << endl;

        if(!pureColorCars.empty()){         // 选择与混色车身相同的颜色
            unordered_map<string, vector<int>>::iterator iter;
            if((iter = pureColorCars.find(ins.cars[mixedColorCars[index]].bodyColor)) != pureColorCars.end()){

            }
            else{
                index = ::rand() % pureColorCars.size();    // 随机选择一个颜色
                iter = pureColorCars.begin();
                for(int i = 0; i != index; ++i){
                    ++iter;
                }
            }
//            unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
//            shuffle(iter->second.begin(), iter->second.end(), default_random_engine(seed));

            int size = iter->second.size();
            for(int i = 0; i != min(5, size); ++i){         // TODO 可能空隙不到 同色/4 那要改为5的倍数
                chro.sequence.emplace_back(iter->second[0]);
                iter->second.erase(iter->second.begin());
            }
            if(iter->second.size() == 0)    pureColorCars.erase(iter);
        }

    }
//    for(auto a: chro.sequence){
//        cout << a << ' ';
//    }cout << endl;
//    evaluation(chro);
//    cout << chro.objs[0] << ' ' << chro.objs[1] << ' ' << chro.objs[2] << endl;
//    cout << "---------------" << endl;

}

void ImprovedNSGA2::cross(vector<chromosome>& population) {
//    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
//    shuffle(population.begin(), population.end(), default_random_engine(seed));
    for(int i = 0; i < population.size(); i += 2){
        vector<int> parent1 = population[i].sequence;
        vector<int> parent2 = population[i+1].sequence;

        if(parent1 != parent2){
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
        }

        evaluation(population[i]);         // 更新child1和child2参数
        evaluation(population[i + 1]);
    }
}

void ImprovedNSGA2::particallyMappedCross(vector<chromosome>& population){
//    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
//    shuffle(population.begin(), population.end(), default_random_engine(seed));
    for(int i = 0; i < population.size(); i += 2){
        vector<int>& parent1 = population[i].sequence;
        vector<int>& parent2 = population[i+1].sequence;
        if(parent1 != parent2) {
            // 生成分段位置 交换位置[index1, index2)
            int index1 = ::rand() % parent1.size();
            int index2 = ::rand() % parent1.size();
            if(index1 == index2)    ++index2;
            if(index1 > index2) swap(index1, index2);

            unordered_map<int, int> map1, map2;         // 记录配对
            vector<int> r1(parent1.size(), 0);
            vector<int> r2(parent2.size(), 0);
            for(int j = index1; j != index2; ++j){
                map1.insert(pair<int, int>{parent1[j], parent2[j]});
                map2.insert(pair<int, int>{parent2[j], parent1[j]});
                swap(parent1[j], parent2[j]);
                r1[parent1[j]] = 1;
                r2[parent2[j]] = 1;
            }
            for(int j = 0; j != parent1.size(); ++j){
                if(j >= index1 and j < index2)  continue;
                if(r1[parent1[j]] != 0){
                    int key = parent1[j];
                    while(r1[map2[key]] == 1){
                        key = map2[key];
                    }
                    parent1[j] = map2[key];
                }
                if(r2[parent2[j]] != 0){
                    int key = parent2[j];
                    while(r2[map1[key]] == 1){
                        key = map1[key];
                    }
                    parent2[j] = map1[key];
                }
            }
            evaluation(population[i]);         // 更新child1和child2参数
            evaluation(population[i + 1]);
        }
        else{
            int randNum = ::rand() % 2;
            if(randNum == 0){
                int index1 = ::rand() % parent1.size();
                int index2 = ::rand() % parent1.size();
                swap(parent1[index1], parent1[index2]);
                swap(parent2[index1], parent2[index2]);
                evaluation(population[i]);         // 更新child1和child2参数
                evaluation(population[i + 1]);
            }
            else{
                vector<chromosome> p1 = {population[i]};
                vector<chromosome> p2 = {population[i+1]};
                particallySwapMutation(p1);
                particallySwapMutation(p2);
                population[i] = p1[0];
                population[i+1] = p2[0];
            }
        }
    }
}

static void getSubRange(const int index, int &start, int &end) {
    // 长度超过 5
    int newStart = index - 2;
    int newEnd = index + 2;
    if (newStart < start) {
        newEnd += (start - newStart);
        newStart = start;
    }
    if (newEnd > end) {
        newStart -= (newEnd - end);
        newEnd = end;
    }
    start = newStart;
    end = newEnd;
}

void ImprovedNSGA2::colorCommonCross(vector<chromosome>& population){
//    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
//    shuffle(population.begin(), population.end(), default_random_engine(seed));
    for(int i = 0; i < population.size(); i += 2){
        for(int j = 0; j != crossTime; ++j){
            vector<int>& parent1 = population[i].sequence;
            vector<int>& parent2 = population[i+1].sequence;
            // 生成两个要交换位置的随机数 index1, index2
            const int index1 = ::rand() % parent1.size();
            // index 落在 [start, end] 区间
            int start1 = index1, end1 = index1;
            findStartEnd(parent1, start1, end1);

            const int index2 = ::rand() % parent1.size();
            int start2 = index2, end2 = index2;
            findStartEnd(parent2, start2, end2);
            int len1 = end1 - start1 + 1;
            int len2 = end2 - start2 + 1;
            if (len1 > 5) {
                // 以 index1 为中心，截取一段
                getSubRange(index1, start1, end1);
            }
            if (len2 > 5) {
                // 以 index2 为中心，截取一段
                getSubRange(index2, start2, end2);
            }

            vector<int> genes1(parent1.begin() + start1, parent1.begin() + end1 + 1);
            unordered_set<int> genes1_set(genes1.begin(), genes1.end()); // make sure find O(1)
            vector<int> genes2(parent2.begin() + start2, parent2.begin() + end2 + 1);
            unordered_set<int> genes2_set(genes2.begin(), genes2.end()); // make sure find O(1)
            vector<int> child1;
            vector<int> child2;

            const int insert_pos1 = end1;
            const int insert_pos2 = end2;

            // 将 genes2 插入 parent1 中
            for (int i = 0; i < parent1.size(); i++) {
                if (genes2_set.find(parent1[i]) == genes2_set.end()) {
                    // 元素跟 genes2 元素不重复
                    child1.emplace_back(parent1[i]);
                }
                if (i == insert_pos1) {
                    // 插入 genes2
                    child1.insert(child1.end(), genes2.begin(), genes2.end());
                }
            }
            // 将 genes1 插入 parent2 中
            for (int i = 0; i < parent2.size(); i++) {
                if (genes1_set.find(parent2[i]) == genes1_set.end()) {
                    // 元素跟 genes2 元素不重复
                    child2.emplace_back(parent2[i]);
                }
                if (i == insert_pos2) {
                    // 插入 genes2
                    child2.insert(child2.end(), genes1.begin(), genes1.end());
                }
            }

            parent1 = child1;
            parent2 = child2;
        }

        evaluation(population[i]);
        evaluation(population[i + 1]);

    }
}

void ImprovedNSGA2::mutation(vector<chromosome> &population) {
    for(int i = 0; i < population.size(); ++i){
        vector<int>& parent = population[i].sequence;
        // 生成位置序列（0-1）
        int oneNum = ::rand() % (parent.size() / 2);    // 变异位置数量
//        oneNum = 2;
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
//        chromosome child = chromosome{parent, vector<double>(4, 0)};
        evaluation(population[i]);         // 更新child参数
//        population[i] = child.objs[0] == std::numeric_limits<double>::infinity() ? population[i] : child;          // 如果为可行解 则保留 否则保留原解
//        population[i] = child;
    }
}

void ImprovedNSGA2::findStartEnd(const vector<int> &sequence, int &start, int &end) const {
    while (start - 1 >= 0) {
        carInfo cur = ins.cars[sequence[start]];
        carInfo prev = ins.cars[sequence[start - 1]];
        if (prev.checkNextNotContinuesColor(cur)) {
            // 颜色不连续
            break;
        }
        --start;
    }
    while (end + 1 <= sequence.size() - 1) {
        carInfo cur = ins.cars[sequence[end]];
        carInfo next = ins.cars[sequence[end + 1]];
        if (cur.checkNextNotContinuesColor(next)) {
            // 颜色不连续
            break;
        }
        ++end;
    }
}

void ImprovedNSGA2::particallySwapMutation(vector<chromosome>& population){
    for(int i = 0; i < population.size(); ++i){
        vector<int> &parent = population[i].sequence;

        // parent = {0, 0, 0, 1, 1, 1, 2, 2, 2, 3};
        // for (auto & p: parent) {
        //     cout << "(" << ins.cars[p].bodyColor << ", " << ins.cars[p].roofColor << ", " << ins.cars[p].carNo << ") ";
        // }
        // cout << endl;

        // 生成两个要交换位置的随机数 index1, index2
        const int index1 = ::rand() % parent.size();
        // index 落在 [start, end] 区间
        int start1 = index1, end1 = index1;
        findStartEnd(parent, start1, end1);

        int index2 = ::rand() % parent.size();
        while (index2 == index1) {
            index2 = ::rand() % parent.size();
        }
        int start2 = index2, end2 = index2;
        if (index2 >= start1 && index2 <= end1) {
            // 落在相同颜色区间里，交换位置
            start1 = index1, end1 = index1;
        } else {
            // 落在颜色不同的区间位置里
            findStartEnd(parent, start2, end2);
            int len1 = end1 - start1 + 1;
            int len2 = end2 - start2 + 1;
            if (len1 > 5) {
                // 以 index1 为中心，截取一段
                getSubRange(index1, start1, end1);
            }
            if (len2 > 5) {
                // 以 index2 为中心，截取一段
                getSubRange(index2, start2, end2);
            }
        }
        if (start2 < start1) {
            // 保证 [start1, end1] < [start2, end2]
            swap(start1, start2);
            swap(end1, end2);
        }

        // cout << start2 << " " << end2 << endl;

        // 存储片段
        vector<int> genes1(parent.begin() + start1, parent.begin() + end1 + 1);
        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        shuffle (genes1.begin(), genes1.end(), std::default_random_engine(seed));
        vector<int> genes2(parent.begin() + start2, parent.begin() + end2 + 1);
        seed = std::chrono::system_clock::now().time_since_epoch().count();
        shuffle (genes2.begin(), genes2.end(), std::default_random_engine(seed));
        vector<int> child1(parent.begin(), parent.begin() + start1);
        vector<int> child2(parent.begin() + end1 + 1, parent.begin() + start2);
        vector<int> child3(parent.begin() + end2 + 1, parent.end());

        // cout << genes1.size() + genes2.size() + child1.size() + child2.size() + child3.size() << endl;
        vector<int> child;
        child.insert(child.end(), child1.begin(), child1.end());
        child.insert(child.end(), genes2.begin(), genes2.end());
        child.insert(child.end(), child2.begin(), child2.end());
        child.insert(child.end(), genes1.begin(), genes1.end());
        child.insert(child.end(), child3.begin(), child3.end());

        parent = child;
        evaluation(population[i]);

        // for (auto & p: parent) {
        //     cout << "(" << ins.cars[p].bodyColor << ", " << ins.cars[p].roofColor << ", " << ins.cars[p].carNo << ") ";
        // }
        // cout << endl;
        // assert(false);
    }
}

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
        sortedSize += front.size();
        if (sortedSize >= needSize) {
            break;
        }
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