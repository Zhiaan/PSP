cmake_minimum_required(VERSION 3.24)
project(PSP)

set(CMAKE_CXX_STANDARD 17)

add_executable(PSP src/main.cpp src/prob/AlgoRunner.cpp src/prob/AlgoRunner.h src/prob/Data.cpp src/prob/Data.h src/prob/Solution.cpp src/prob/Solution.h src/comm/IO.cpp src/comm/IO.h src/comm/Param.cpp src/comm/Param.h src/prob/model/Greedy.cpp src/prob/model/Greedy.h)
