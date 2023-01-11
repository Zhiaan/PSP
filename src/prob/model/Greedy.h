//
// Created by Zhiaan on 2023/1/11.
//

#ifndef PSP_GREEDY_H
#define PSP_GREEDY_H
#include "../Data.h"
#include <iostream>
#include <algorithm>
#include "../Solution.h"

class Greedy {
public:
    solution greedyRunner();
    Greedy(instance);
private:
    instance ins;
};


#endif //PSP_GREEDY_H
