//
// Created by Zhiaan on 2023/1/11.
//

#include "Greedy.h"
Greedy::Greedy(instance inst) {
    ins = inst;
}

solution Greedy::greedyRunner() {
    // TODO 输入实例ins 输出解solution
    // struct instance在Data.h .csv文件中的所有内容
    // struct solution在Solution.h
    // 下面的cout可以查看ins的信息
//    cout << ins.instanceNo << endl;
//    for(auto i: ins.cars) {
//        cout << i.carNo << ' ' << i.type << ' ' << i.bodyColor << ' ' << i.roofColor << ' ' << i.materialNo << ' ' << i.engine << ' ' << i.speedTrans << endl;
//    }
    solution s;

    // TODO 在这里写就可以 输出写好了



//    solution test;
//    for(int i = 0; i != 103; ++i){
//        test.sequence.emplace_back(i);
//    }
//
//    for(int i: test.sequence){
//        cout << i << ' ';
//    }cout << endl;
    evaluation(s);

    return s;
}

void Greedy::evaluation(solution &s) {
    s.obj1 = 0;
    s.obj2 = 0;
    s.obj3 = 0;     // 三目标值归零

    int typeCommonTime = 1;
    int colorCommonTime = 0;
    for(int i = 0; i != s.sequence.size() - 1; ++i){
        // obj1 焊装总等待时间
        if(ins.cars[s.sequence[i]].type == ins.cars[s.sequence[i + 1]].type){   // 如果前后相等 记录连续相等车数
            ++typeCommonTime;
        }
        else{
            if(typeCommonTime < ins.weldingContinueTime / ins.weldingTime){     // 如果前后不相等且小于30min
                s.obj1 += ins.weldingContinueTime - typeCommonTime * ins.weldingTime;
            }
            typeCommonTime = 1;
        }

        // obj2 切换喷头次数
        if(ins.cars[s.sequence[i]].roofColor != "无对比颜色" and ins.cars[s.sequence[i]].roofColor != ins.cars[s.sequence[i]].bodyColor){     // 本车车顶颜色!=车身颜色
            ++s.obj2;
        }
        else{               // 本车车顶颜色=车身颜色
            ++colorCommonTime;
            if(colorCommonTime == 5){       // 如果五辆车颜色相同
                ++s.obj2;
                colorCommonTime = 0;
                continue;
            }

        }
        // 如果前车车身!=后车车顶
        if(ins.cars[s.sequence[i]].bodyColor != (ins.cars[s.sequence[i + 1]].roofColor == "无对比颜色" ? ins.cars[s.sequence[i + 1]].bodyColor : ins.cars[s.sequence[i + 1]].roofColor)){
            ++s.obj2;
            colorCommonTime = 0;
        }

    }
    if(ins.cars[*(s.sequence.end()-1)].roofColor != "无对比颜色" and ins.cars[*(s.sequence.end()-1)].roofColor != ins.cars[*(s.sequence.end()-1)].bodyColor){     // 最后一辆车的车顶颜色!=车身颜色
        ++s.obj2;
    }

    // obj3
    s.obj3 += s.obj1 + ins.weldingTime * s.sequence.size() + s.obj2 * ins.paintingWaitingTime + ins.paintingTime * 2 * s.sequence.size() + ins.assembleTime * s.sequence.size();


}
