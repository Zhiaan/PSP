//
// Created by Zhiaan on 2023/1/12.
//

#include <random>
#include "ImprovedNSGA2.h"
ImprovedNSGA2::ImprovedNSGA2(instance inst) {
    ins = inst;
    populationSize = 500;
    chromosomeLength = ins.cars.size();
}

vector<solution> ImprovedNSGA2::NSGA2Runner() {
    vector<solution> solutions;

    vector<chromosome> population(populationSize);
    initializePopulation(population);           // 初始化种群



//    for(auto i: population){
//        for(auto j: i.sequence){
//            cout << j << ' ';
//        }cout << endl;
//        cout << i.objs[0] << ' ' << i.objs[1] << ' ' << i.objs[2] << endl;
//    }

    return solutions;
}

void ImprovedNSGA2::evaluation(chromosome &c) {
    c.objs = vector<double>(3, 0);  // 三目标值归零

    int typeCommonTime = 1;
    int colorCommonTime = 0;
    int speedTransCommonTime = 0;   // 连续四驱次数
    for(int i = 0; i != c.sequence.size() - 1; ++i){
        // 记录四驱连续次数
        if(ins.cars[c.sequence[i]].speedTrans == "四驱"){
            ++speedTransCommonTime;
            if(speedTransCommonTime == 4){  // 连续四驱数量到4 将目标值设为极大值
                c.objs[0] = c.objs[1] = c.objs[2] = INT_MAX / 2;
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
        c.objs[0] = c.objs[1] = c.objs[2] = INT_MAX / 2;
        goto label;
    }
    if(ins.cars[*(c.sequence.end()-1)].roofColor != "无对比颜色" and ins.cars[*(c.sequence.end()-1)].roofColor != ins.cars[*(c.sequence.end()-1)].bodyColor){     // 最后一辆车的车顶颜色!=车身颜色
        ++c.objs[1];
    }

    // obj3
    c.objs[2] += c.objs[0] + ins.weldingTime * c.sequence.size() + c.objs[1] * ins.paintingWaitingTime + ins.paintingTime * 2 * c.sequence.size() + ins.assembleTime * c.sequence.size();
    label:
    c.objs[0] = c.objs[1] = c.objs[2] = INT_MAX / 2;
    return ;

}

void ImprovedNSGA2::initializePopulation(vector<chromosome>& population) {
    for(auto& chro: population){
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
    }
}