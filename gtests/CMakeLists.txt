cmake_minimum_required(VERSION 3.26)

if (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    find_package(GTest REQUIRED)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(VCPKG  "C:/Users/user/vcpkg/installed/x64-windows")
    find_package(GTest PATHS ${VCPKG})
endif ()

include_directories(GTest)
include_directories(${GTest_INCLUDE_DIRS})

enable_testing()
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
include(GoogleTest)
gtest_discover_tests(google_test)
