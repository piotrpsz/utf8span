cmake_minimum_required(VERSION 3.27)
project(utf8span)
set(CMAKE_CXX_STANDARD 20)

add_subdirectory(tests)

add_library(utf8span STATIC
        scope.cpp scope.h
        murmur.cpp murmur.h
        span.cpp span.h
)
