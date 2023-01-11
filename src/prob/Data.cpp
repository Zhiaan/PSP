//
// Created by Zhiaan on 2023/1/11.
//

#include "Data.h"

vector<string> Data::getPathList(Param p) {
    string dirpath = p.dataPath;
    DIR *dir = opendir(dirpath.c_str());
    if (dir == NULL) {
        cout << "opendir error" << endl;
    }

    vector<string> allPath;
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR) {//It's dir
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;
            string dirNew = dirpath + "/" + entry->d_name;
            vector<string> tempPath = getPathList(p);
            allPath.insert(allPath.end(), tempPath.begin(), tempPath.end());

        } else {
            //cout << "name = " << entry->d_name << ", len = " << entry->d_reclen << ", entry->d_type = " << (int)entry->d_type << endl;
            string name = entry->d_name;
            string imgdir = dirpath + "/" + name;
            //sprintf("%s",imgdir.c_str());
            allPath.push_back(imgdir);
        }
    }
    closedir(dir);
    //system("pause");
    return allPath;
}

vector<instance> Data::getInstance(vector<string> filePathList) {
    vector<instance> instances = {};
    for(string filePath: filePathList) {
        IO io;
        vector<vector<string>> file = io.readCSV(filePath);
        instance ins;
        ins.instanceNo = filePath.substr(32, filePath.size() == 44 ? 8 : 9);
        ins.cars = {};
        for(auto i: file){
            carInfo c;
            c.carNo = stoi(i[0]);
            c.type = i[1];
            c.bodyColor = i[2];
            c.roofColor = i[3];
            c.materialNo = i[4];
            c.engine = i[5];
            c.speedTrans = i[6];
            ins.cars.emplace_back(c);
        }
        instances.emplace_back(ins);
    }
    return instances;
}