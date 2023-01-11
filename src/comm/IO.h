//
// Created by Zhiaan on 2023/1/11.
//

#ifndef PSP_IO_H
#define PSP_IO_H
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdarg.h>
#include <sys/stat.h>
using namespace std;

class IO {
public:
    vector<string> readTXT(string);    // 读TXT
    vector<string> splitString(string);     // 按空格分割 将数据读入

    vector<vector<string>> readCSV(string path);    // 读CSV
    void writeTXT(string path, string content);      // 输出TXT
    void writeCSV(string path, string content);     // 输出CSV
};


#endif //PSP_IO_H
