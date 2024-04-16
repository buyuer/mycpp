cmake_minimum_required(VERSION 3.28)
project(mycpp)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(mycpp)
find_package(Threads)

add_executable(mycpp_test)
# target_link_libraries(mycpp_test PUBLIC mycpp)
target_sources(mycpp_test PUBLIC
        FILE_SET all_modules
        TYPE CXX_MODULES
        FILES ${MYCPP_MODULES_FILES})
target_sources(mycpp_test PUBLIC main.cpp)
