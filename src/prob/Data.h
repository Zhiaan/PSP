//
// Created by Zhiaan on 2023/1/11.
//

#ifndef PSP_DATA_H
#define PSP_DATA_H
#include <string>
#include <vector>
#include <iostream>
#include <cmath>
#include "../comm/IO.h"
#include "../comm/Param.h"
#include <memory.h>
#include <dirent.h>
using namespace std;

struct carInfo{
    int carNo;  // 车编号
    string type;    // 车型  A/B
    string bodyColor;   // 车身颜色
    string roofColor;   // 车顶颜色
    string materialNo;  // 物料编号
    string engine;      // 发动机  燃油/混动
    string speedTrans;  // 变速器  四驱/两驱

    inline bool checkRoofNotEqualBody() const {
        return roofColor != "无对比颜色" and roofColor != bodyColor;
    }

    inline bool checkBodyNotEqualNextRoof(const carInfo &nextCar) const {
        return bodyColor != (nextCar.roofColor == "无对比颜色" ? nextCar.bodyColor : nextCar.roofColor);
    }

    inline bool checkNextNotContinuesColor(const carInfo &nextCar) const {
        // 相邻两辆车在喷漆过程中若发生了颜色切换就认为颜色不连续。
        return checkRoofNotEqualBody() or checkBodyNotEqualNextRoof(nextCar) or nextCar.checkRoofNotEqualBody();
    }
};
struct instance{
    string instanceNo;      // 实例编号
    vector<carInfo> cars;
    long long weldingTime = 80.0;  // 焊装时间/车
    long long weldingContinueTime = 30 * 60;   // 焊装持续时间30min
    long long paintingTime = 40.0; //车顶/车身涂装时间
    long long paintingWaitingTime = 80.0;  // 涂装切换时间
    long long assembleTime = 80.0;     // 总装时间/车
    int threadId; // 当前处理这个 instance 的线程号，方便输出调试
};

class Data {
public:
    string filename;
    vector<string> getPathList(Param p);        // 获得所有算例的路径
    instance getInstance(const string &);       // 获得算例集合
};


#endif //PSP_DATA_H
