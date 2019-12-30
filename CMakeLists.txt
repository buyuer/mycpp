cmake_minimum_required(VERSION 3.15)
project(mycpp)

set(CMAKE_CXX_STANDARD 17)

include_directories(src/)

add_executable(mycpp main.cpp)