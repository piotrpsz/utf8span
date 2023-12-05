cmake_minimum_required(VERSION 3.26)

#find_library(utf8span ${CMAKE_CURRENT_BINARY_DIR})

add_executable(google_test
        scope_test.cc
        scope_test.h
        ../scope.cpp ../scope.h
        ../span.cpp ../span.h
        ../murmur.cpp ../murmur.h
)
target_link_libraries(google_test PUBLIC
        GTest::gtest GTest::gtest_main
)
target_include_directories(google_test PUBLIC
        GTest
)
gtest_discover_tests(google_test)
