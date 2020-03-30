cmake_minimum_required(VERSION 3.15)
project(mycpp)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lpthread")

include_directories(src/)

add_executable(mycpp main.cpp)