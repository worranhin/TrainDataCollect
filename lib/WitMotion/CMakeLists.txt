cmake_minimum_required(VERSION 3.10) # 设置CMake的最低版本需求

project(WitMotion VERSION 0.1) # 定义项目名称

# # 设置C标准
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_library(WitMotion wit_c_sdk.c Com.cpp)