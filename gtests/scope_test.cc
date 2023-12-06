#include "scope_test.h"
#include "../scope.h"
#include "../span.h"
#include <iostream>

TEST_F(ScopeTest, prev) {
    auto dt = executor([&] {
        struct except_t {
            std::string c;
            int size;
            int len;
        };

        struct {
            std::string text;
            std::vector<except_t> except;
        } tests[] = {
                "", {},
                "ó", {{"ó", 2, 1}},
                "ół", {{"ł", 2, 1}, {"ó", 2, 1}},
                "ółko", {{"o", 1, 1}, {"k", 1, 1}, {"ł", 2, 1}, {"ó", 2, 1}}
        };

        for (auto const& tt: tests) {
            ppx::utf8::span span{tt.text.c_str(), ppx::utf8::uint(tt.text.size())};
            span.end();

            int i = 0;
            for (;; i++) {
                auto scope = span.prev();
                if (!scope.valid_chars()) break;
                ASSERT_EQ(scope.str(), tt.except[i].c);
                ASSERT_EQ(scope.size(), tt.except[i].size);
                ASSERT_EQ(scope.len(), tt.except[i].len);
            }
            ASSERT_EQ(i, tt.except.size());
        }
    });
    std::cout.fill('.');
    std::cout << std::setw(50) << std::left << "ScopeTest::prev " << dt << '\n';
}

TEST_F(ScopeTest, next) {
    auto dt = executor([&] {
        struct except_t {
            std::string c;
            ppx::utf8::uint size;
            ppx::utf8::uint len;
        };

        struct {
            std::string text;
            std::vector<except_t> except;
        } tests[] = {
                "", {},
                "ó", {{"ó", 2, 1}},
                "ół", {{"ó", 2, 1}, {"ł", 2, 1}},
                "ółko", {{"ó", 2, 1}, {"ł", 2, 1}, {"k", 1, 1}, {"o", 1, 1}}
        };

        for (auto const& tt: tests) {
            ppx::utf8::span span{tt.text.c_str(), ppx::utf8::uint(tt.text.size())};
            int i = 0;
            for (;; i++) {
                auto scope = span.next();
                if (!scope.valid_chars()) break;
                ASSERT_EQ(scope.str(), tt.except[i].c);
                ASSERT_EQ(scope.size(), tt.except[i].size);
                ASSERT_EQ(scope.len(), tt.except[i].len);
            }
            ASSERT_EQ(i, tt.except.size());
        }
    });
    std::cout.fill('.');
    std::cout << std::setw(50) << std::left << "ScopeTest::next " << dt << '\n';
}

TEST_F(ScopeTest, skip) {
    auto dt = executor([&] {
        struct {
            std::string text;
            int n;
            bool except;
        } tests[] = {
                "", 0, false,
                "ó", 0, true,
                "ó", 1, true,
                "ó", 2, false,
                "ół", 1, true,
                "ół", 2, true,
                "ół", 3, false,
                "ółk", 0, true,
                "ółk", 1, true,
                "ółk", 2, true,
                "ółk", 3, true,
                "ółk", 4, false,
        };

        for (auto const& tt: tests) {
            ppx::utf8::span span{tt.text.c_str(), ppx::utf8::uint(tt.text.size())};
            ASSERT_EQ(span.skip(tt.n), tt.except);
        }
    });

    std::cout.fill('.');
    std::cout << std::setw(50) << std::left << "ScopeTest::skip " << dt << '\n';
}

TEST_F(ScopeTest, index_operator) {
    auto dt = executor([&] {
        ppx::utf8::span span{text_two_};

        struct {
            ppx::utf8::uint i;
            std::string expected;
        } tests[] = {
                {0,  "Ł"},
                {1,  "ó"},
                {2,  "d"},
                {3,  "ź"},
                {9,  "z"},
                {10, "ó"},
                {11, "ł"},
                {12, "k"},
        };

        for (auto const& tt: tests) {
            auto scope = span[tt.i];
            ASSERT_EQ(scope.str(), tt.expected);
        }
    });
    std::cout.fill('.');
    std::cout << std::setw(50) << std::left << "ScopeTest::index_operator " << dt << '\n';
}

TEST_F(ScopeTest, subspan2) {
    auto dt = executor([&] {
        ppx::utf8::span span{text_two_};

        struct {
            ppx::utf8::uint start;
            ppx::utf8::uint len;
            std::string expected;
        } tests[] = {
                {0, 0,  ""},
                {0, 1,  "Ł"},
                {0, 2,  "Łó"},
                {0, 3,  "Łód"},
                {0, 4,  "Łódź"},
                {8, 0,  ""},
                {8, 1,  "c"},
                {8, 2,  "cz"},
                {8, 3,  "czó"},
                {8, 4,  "czół"},
                {8, 5,  "czółk"},
                {8, 6,  "czółko"},
                {8, 7,  "czółkow"},
                {8, 8,  "czółkows"},
                {8, 9,  "czółkowsk"},
                {8, 10, "czółkowski"},
                {8, 11, ""},
        };

        for (auto const& tt: tests) {
            auto scope = span.subspan(tt.start, tt.len);
            ASSERT_EQ(scope.str(), tt.expected);
        }
    });
    std::cout.fill('.');
    std::cout << std::setw(50) << std::left << "ScopeTest::subspan2 " << dt << '\n';
}

TEST_F(ScopeTest, subspan1) {
    auto dt = executor([&] {
        ppx::utf8::span span{text_one_};

        struct {
            ppx::utf8::uint start;
            std::string expected;
        } tests[] = {
                {0, "Łódź"},
                {1, "ódź"},
                {2, "dź"},
                {3, "ź"},
                {4, ""}
        };

        for (auto const& tt: tests) {
            auto scope = span.subspan(tt.start);
            ASSERT_EQ(scope.str(), tt.expected);
        }
    });
    std::cout.fill('.');
    std::cout << std::setw(50) << std::left << "ScopeTest::subspan1 " << dt << '\n';
}

TEST_F(ScopeTest, search) {
    auto dt = executor([&] {
        ppx::utf8::span span{text_one_};

        struct {
            std::string c;
            ppx::utf8::uint pos;
        } positive_tests[] = {
                {"ź", 3},
                {"d", 2},
                {"ó", 1},
                {"Ł", 0},
        };

        struct {
            std::string c;
            int pos;
        } negatime_tests[] = {
                {"ź", 100},
                {"d", 101},
                {"ó", 102},
                {"Ł", 103},
                {"Ó", 104},
        };

        for (auto const& tt: positive_tests) {
            auto scope = ppx::utf8::span{tt.c}();
            auto opt = span.search(scope, int(tt.pos));
            ASSERT_TRUE(opt.has_value());
            auto [pos, found_scope] = *opt;
            ASSERT_EQ(tt.pos, pos);
            ASSERT_EQ(tt.c, found_scope.str());
        }
        for (auto const& tt: negatime_tests) {
            auto scope = ppx::utf8::span{tt.c}();
            auto opt = span.search(scope, tt.pos);
            ASSERT_FALSE(opt.has_value());
        }
    });
    std::cout.fill('.');
    std::cout << std::setw(50) << std::left << "ScopeTest::search " << dt << '\n';
}

TEST_F(ScopeTest, tolower_test) {
    auto dt = executor([&] {
        struct {
            std::string text;
            std::string expected;
        } tests[] = {
                {"ŁÓDŹ ĄĘĆŃ Żnin", "łódź ąęćń żnin"},
                {"ẞÄÖÜ ẞ Ä Ö Ü",   "ßäöü ß ä ö ü"}
        };

        for (auto const& tt: tests) {
            auto rv = ppx::utf8::span{tt.text}().to_lower();
            //auto rv = scope.to_lower();
            ASSERT_EQ(std::string(rv.data(), rv.size()), tt.expected);
        }
    });
    std::cout.fill('.');
    std::cout << std::setw(50) << std::left << "ScopeTest::to_lower " << dt << '\n';
}

TEST_F(ScopeTest, toupper) {
    auto dt = executor([&] {
        struct {
            std::string text;
            std::string expected;
        } tests[] = {
                {"ßäöü ß ä ö ü",   "ẞÄÖÜ ẞ Ä Ö Ü"},
                {"łódź ąęćń żnin", "ŁÓDŹ ĄĘĆŃ ŻNIN",},
        };

        for (auto const& tt: tests) {
            auto rv = ppx::utf8::span{tt.text}().to_upper();
            ASSERT_EQ(std::string(rv.data(), rv.size()), tt.expected);
        }
    });
    std::cout.fill('.');
    std::cout << std::setw(50) << std::left << "ScopeTest::to_upperr " << dt << '\n';
}
