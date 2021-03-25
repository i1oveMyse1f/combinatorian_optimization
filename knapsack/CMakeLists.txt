cmake_minimum_required(VERSION 3.16)
project(knapsack)

set(CMAKE_CXX_STANDARD 20)
#set(CMAKE_CXX_STANDARD 11)

add_executable(knapsack src/knapsack.h src/knapsack.cpp src/solution_center_optimization.cpp)
#add_executable(knapsack src/submit_to_system.cpp)