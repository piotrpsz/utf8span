cmake_minimum_required(VERSION 3.26)
project(utf8span)
set(CMAKE_CXX_STANDARD 20)

enable_testing()
find_package(GTest REQUIRED)
include_directories(GTest)
include(GoogleTest)


#find_package(GTest REQUIRED)
#message("found: ${GTest_FOUND}")
#message("both: ${GTEST_BOTH_LIBRARIES}")
#message("lib: ${GTEST_LIBRARIES}")
#message("lib: ${GTEST_MAIN_LIBRARIES}")

add_subdirectory(gtests)
#include_directories(GTest)

add_library(utf8span STATIC
        scope.cpp scope.h
        murmur.cpp murmur.h
        span.cpp span.h
)
