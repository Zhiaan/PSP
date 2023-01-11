//
// Created by Zhiaan on 2023/1/11.
//

#include "IO.h"
using namespace std;
vector<string> IO::readTXT(string path) {
    vector<string> lines = {};       // 记录读取的行
    ifstream infile;
    infile.open(path, ios::in);
    if (!infile.is_open()) {    // 若无此文件 返回空
        return vector<string>{};
    }
    string buf;
    while (getline(infile, buf)) {
        lines.emplace_back(buf);
    }
    return lines;
}


vector<string> IO::splitString(string s){
    istringstream ss(s);
    vector<string> words;
    string word;
    while(ss >> word) {
        words.push_back(word);
    }
    return words;
}

vector<vector<string>> IO::readCSV(string path){
    ifstream inFile(path, ios::in);
    string lineStr;
    vector<vector<string>> strArray;
    int flag = 0;       // 删除表头
    while (getline(inFile, lineStr)){
        if(flag++ == 0){
            continue;
        }
        // 存成二维表结构
        stringstream ss(lineStr);
        string str;
        vector<string> lineArray;
        // 按照逗号分隔
        while (getline(ss, str, ','))
            lineArray.push_back(str);
        strArray.push_back(lineArray);

    }

    return strArray;
}

void IO::writeTXT(string path, string content) {
    ofstream out;
    out.open(path); //利用构造函数创建txt文本，并且打开该文本
    out << content;
    out.close();
}

void IO::writeCSV(string path, string content) {
    // 总统计文件
    ofstream oFile;
    if(content.substr(0, 1) == "V"){
        oFile.open(path, ios::out | ios::trunc);
        oFile << content << endl;
        oFile.close();
        return;
    }
    oFile.open(path, ios::out | ios::app);
    oFile << content << endl;
    oFile.close();
}
