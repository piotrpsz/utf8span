#pragma one
#include <gtest/gtest.h>
#include <string>
#include <sstream>

template<typename Fn, typename... Args>
static inline std::string
executor(Fn fn, Args &&... args) {
    auto start = std::chrono::steady_clock::now();
    fn(std::forward<Args>(args)...);
    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> const elapsed = end - start;

    std::stringstream ss{};
    ss << elapsed.count() << 's';
    return ss.str();
}

class ScopeTest : public testing::Test {
protected:
    std::string const text_one_{ "Łódź"};
    std::string const text_two_{ "Łódź Pszczółkowski"};
};
