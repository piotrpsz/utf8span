cmake_minimum_required(VERSION 3.26)

enable_testing()
find_package(GTest REQUIRED)
include_directories(GTest)
include_directories(${GTest_INCLUDE_DIRS})
include(GoogleTest)

add_executable(google_test
        scope_test.cc
        scope_test.h
        ../scope.cpp
        ../span.cpp
        ../murmur.cpp
)

target_link_libraries(google_test PUBLIC
        GTest::gtest GTest::gtest_main
)
target_include_directories(google_test PUBLIC
        GTest
)
gtest_discover_tests(google_test)
