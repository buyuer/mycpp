cmake_minimum_required(VERSION 3.15)
project(mycpp)

set(CMAKE_CXX_STANDARD 20)

find_package(Threads)

include_directories(./)
include_directories(mycpp/)

add_executable(mycpp main.cpp)
target_link_libraries(mycpp ${CMAKE_THREAD_LIBS_INIT})

add_subdirectory(test)