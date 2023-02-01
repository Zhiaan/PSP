cmake_minimum_required(VERSION 3.16)
project(PSP)

set(CMAKE_CXX_STANDARD 17)

SET(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=c++17 -pthread")

add_executable(PSP src/main.cpp src/prob/AlgoRunner.cpp src/prob/AlgoRunner.h src/prob/Data.cpp src/prob/Data.h src/prob/Solution.cpp src/prob/Solution.h src/comm/IO.cpp src/comm/IO.h src/comm/Param.cpp src/comm/Param.h src/prob/model/ImprovedNSGA2.h src/prob/model/ImprovedNSGA2.cpp src/prob/model/Greedy.cpp src/prob/model/Greedy.h src/prob/model/Obj1Greedy.cpp src/prob/model/Obj1Greedy.h src/prob/model/Obj2Greedy.cpp src/prob/model/Obj2Greedy.h)
