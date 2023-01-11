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
};
struct instance{
    string instanceNo;      // 实例编号
    vector<carInfo> cars;
    double weldingTime = 80.0;  // 焊装时间/车
    double weldingContinueTime = 30 * 60;   // 焊装持续时间30min
    double paintingTime = 40.0; //车顶/车身涂装时间
    double paintingWaitingTime = 80.0;  // 涂装切换时间
    double assembleTime = 80.0;     // 总装时间/车
};

class Data {
public:
    string filename;
    vector<string> getPathList(Param p);        // 获得所有算例的路径
    vector<instance> getInstance(vector<string>);       // 获得算例集合
};


#endif //PSP_DATA_H
