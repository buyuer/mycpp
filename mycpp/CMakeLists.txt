cmake_minimum_required(VERSION 3.28)

file(GLOB_RECURSE MYCPP_MODULE_FILES *.ixx)
file(GLOB_RECURSE MYCPP_SOURCE_FILES *.cpp *.cxx *.cc)
set(MYCPP_MODULE_FILES ${MYCPP_MODULE_FILES} PARENT_SCOPE)
set(MYCPP_SOURCE_FILES ${MYCPP_SOURCE_FILES} PARENT_SCOPE)

add_library(mycpp OBJECT)
set_target_properties(mycpp PROPERTIES
        CXX_STANDARD 20
        CXX_EXTENSIONS OFF
        CXX_STANDARD_REQUIRED ON)
target_sources(mycpp PUBLIC
        FILE_SET all_modules
        TYPE CXX_MODULES
        FILES ${MYCPP_MODULE_FILES})