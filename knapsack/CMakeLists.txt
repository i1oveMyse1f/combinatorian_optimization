cmake_minimum_required(VERSION 3.16)
project(knapsack)

set(CMAKE_CXX_STANDARD 20)
#set(CMAKE_CXX_STANDARD 11)

add_executable(knapsack src/solution_meet_in_the_middle.cpp src/knapsack.h src/knapsack.cpp)
#add_executable(knapsack src/submit_to_system.cpp)