//
// Created by Zhiaan on 2023/2/15.
//

#include "Obj2Greedy.h"

Obj2Greedy::Obj2Greedy(instance inst, vector<int> sequence) {
    ins = inst;
    neighborSize = 500;
    maxIterTime = 200;
    existSolution = {sequence, 0, 0, 0, 0};
}

vector<solution> Obj2Greedy::Obj2GreedyRunner() {
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
        bestNeighbor.obj2 = INT_MAX;
        for(int j = 0; j != neighborSize; ++j){
            sol neighbor = localBestSolution;

            particallySwapMutation(neighbor);
            if((neighbor.obj2 < bestNeighbor.obj2) \
 or (neighbor.obj2 == bestNeighbor.obj2 and neighbor.obj1 <= bestNeighbor.obj1 and
     ((neighbor.obj3 <= bestNeighbor.obj3 and neighbor.obj4 < bestNeighbor.obj4) or (neighbor.obj3 < bestNeighbor.obj3 and neighbor.obj4 <= bestNeighbor.obj4)))) bestNeighbor = neighbor;

//            cout << neighbor.obj4 << ' ' << bestNeighbor.obj4 << endl;
        }
        localBestSolution = bestNeighbor;
        if((bestNeighbor.obj2 < globalBestSolution.obj2) \
 or (bestNeighbor.obj2 == globalBestSolution.obj2 and bestNeighbor.obj1 <= globalBestSolution.obj1 and
     ((bestNeighbor.obj3 <= globalBestSolution.obj3 and bestNeighbor.obj4 < globalBestSolution.obj4) or (bestNeighbor.obj3 < globalBestSolution.obj3 and bestNeighbor.obj4 <= globalBestSolution.obj4)))) {
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

sol Obj2Greedy::generateSolution(){
    sol initSolution;
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

    auto emplace_helper = [](sol &chro, vector<vector<carInfo>> &A_same_copy, vector<carInfo> &A_not_same_copy, vector<vector<carInfo>> &B_same_copy, vector<carInfo> &B_not_same_copy) {
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
            cout << "error" << endl;
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
        emplace_helper(initSolution, A_same_copy, A_not_same_copy, B_same_copy, B_not_same_copy);
    } else {
        // B 放前面
        emplace_helper(initSolution, B_same_copy, B_not_same_copy, A_same_copy, A_not_same_copy);
    }

    evaluation(initSolution);

    return initSolution;
}


void Obj2Greedy::particallySwapMutation(sol& population){
    auto findStartEnd = [&](const vector<int> &sequence, int &start, int &end) {
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
    };
    auto getSubRange = [](const int index, int &start, int &end) {
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
    };

    vector<int> &parent = population.sequence;

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
    evaluation(population);

    // for (auto & p: parent) {
    //     cout << "(" << ins.cars[p].bodyColor << ", " << ins.cars[p].roofColor << ", " << ins.cars[p].carNo << ") ";
    // }
    // cout << endl;
    // assert(false);
}

void Obj2Greedy::evaluation(sol &c) {
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
