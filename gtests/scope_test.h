#pragma one
#include <gtest/gtest.h>
#include <string>

class ScopeTest : public testing::Test {
protected:
    std::string const text_one_{ "Łódź"};
    std::string const text_two_{ "Łódź Pszczółkowski"};
};
