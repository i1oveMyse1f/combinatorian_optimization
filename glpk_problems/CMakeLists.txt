cmake_minimum_required(VERSION 3.16)
project(glpk_problems)

set(CMAKE_CXX_STANDARD 11)

add_executable(glpk_problems main.cpp)

target_link_libraries(glpk_problems "glpk")
