#include "scope.h"

namespace ppx::utf8 {
    // https://design215.com/toolbox/ascii-utf8.php
    const std::unordered_map<u16, std::vector<u8>> scope::Uppercase2 {
            // polish
            {0x84c4, {0xc4, 0x85}},        // Ą -> ą
            {0x86c4, {0xc4, 0x87}},        // Ć -> ć
            {0x98c4, {0xc4, 0x99}},        // Ę -> ę
            {0x81c5, {0xc5, 0x82}},        // Ł -> ł
            {0x83c5, {0xc5, 0x84}},        // Ń -> ń
            {0x93c3, {0xc3, 0xb3}},        // Ó -> ó
            {0x9ac5, {0xc5, 0x9b}},        // Ś -> ś
            {0xb9c5, {0xc5, 0xba}},        // Ź -> ź
            {0xbbc5, {0xc5, 0xbc}},        // Ż -> ż
            // german
            {0x84c3, {0xc3, 0xa4}},        // Ä -> ä
            {0x96c3, {0xc3, 0xb6}},        // Ö -> ö
            {0x9cc3, {0xc3, 0xbc}},        // Ü -> ü
    };
    const std::unordered_map<u32, std::vector<u8>> scope::Uppercase3 {
            // german
            {0x9ebae1, {0xc3, 0x9f}}       // ẞ -> ß
    };
    const std::unordered_map<u16, std::vector<u8>> scope::SmallLetters{
            // polish
            {0x85c4, {0xc4, 0x84}},      // ą -> Ą
            {0x87c4, {0xc4, 0x86}},      // ć -> Ć
            {0x99c4, {0xc4, 0x98}},      // ę -> Ę
            {0x82c5, {0xc5, 0x81}},      // ł -> Ł
            {0x84c5, {0xc5, 0x83}},      // ń -> Ń
            {0xb3c3, {0xc3, 0x93}},      // ó -> Ó
            {0x9bc5, {0xc5, 0x9a}},      // ś -> Ś
            {0xbac5, {0xc5, 0xb9}},      // ź -> Ź
            {0xbcc5, {0xc5, 0xbb}},      // ż -> Ż
            // german
            {0xa4c3, {0xc3, 0x84}},      // ä -> Ä
            {0xb6c3, {0xc3, 0x96}},      // ö -> Ö
            {0xbcc3, {0xc3, 0x9c}},      // ü -> Ü
            {0x9fc3, {0xe1, 0xba, 0x9e}},// ß -> ẞ
    };

    // operator+=
    scope& scope::operator+=(scope const& rhs) noexcept {
        // The beginning of rhs must be the end of me.
        // The added scopes must create continuity.
        if ((addr_ + size_) == rhs.addr_) {
            size_ += rhs.size_;
            if (len_ != -1 && rhs.len_ != -1)
                len_ += rhs.len_;
        }
        return *this;
    }

    // to_upper
    std::vector<char> scope::
    to_upper() const noexcept {
        std::vector<char> vec;
        vec.reserve(size_);

        auto ptr = addr_;
        auto const sentinel = addr_ + size_;
        for (int cp_counter = 0; cp_counter < len_ && ptr < sentinel; cp_counter++) {
            switch (codepoint_size(ptr)) {
                case 1:
                    vec.push_back(char(std::toupper(*ptr++)));
                    break;
                case 2: {
                    auto const w = *(u16 *) ptr;
                    if (auto it = SmallLetters.find(w); it != SmallLetters.end()) {
                        for (auto const c: it->second)
                            vec.push_back(char(c));
                        ptr += 2;
                    } else {
                        vec.push_back(*ptr++);
                        vec.push_back(*ptr++);
                    }
                    break;
                }
                case 3:
                    vec.push_back(*ptr++);
                    vec.push_back(*ptr++);
                    vec.push_back(*ptr++);
                    break;
            }
        }
        vec.shrink_to_fit();
        return vec;
    }
    [[nodiscard]] std::vector<char> scope::
    to_lower() const noexcept {
        std::vector<char> vec;
        vec.reserve(size_);

        auto ptr = addr_;
        auto const sentinel = addr_ + size_;
        for (int i = 0; i < len_ && ptr < sentinel; i++) {
            switch (codepoint_size(ptr)) {
                case 1:
                    vec.push_back(char(std::tolower(*ptr++)));
                    break;
                case 2: {
                    auto const w = *(u16 *) ptr;
                    if (auto it = Uppercase2.find(w); it != Uppercase2.end()) {
                        for (auto const c: it->second)
                            vec.push_back(char(c));
                        ptr += 2;
                    } else {
                        vec.push_back(*ptr++);
                        vec.push_back(*ptr++);
                    }
                    break;
                }
                case 3: {
                    auto const w = (*(u32 *) ptr) & 0x00ffffff;
                    if (auto it = Uppercase3.find(w); it != Uppercase3.end()) {
                        for (auto c: it->second)
                            vec.push_back(char(c));
                        ptr += 3;
                    } else {
                        vec.push_back(*ptr++);
                        vec.push_back(*ptr++);
                        vec.push_back(*ptr++);
                    }
                    break;
                }
            }
        }
        vec.shrink_to_fit();
        return vec;
    }

    // hash of scope
    u32 scope::
    hash() const noexcept {
        return murmur::hash3(addr_, size_, 10);
    }

    uint scope::
    codepoint_size(char const *const ptr) noexcept {
        auto const c = static_cast<u8>(*ptr);
        if (c >= 0xc2 && c <= 0xdf) return 2;
        if (c >= 0xe0 && c <= 0xef) return 3;
        return 1;
    }

    bool
    scope::is_word_boundary() const noexcept {
        switch (size_) {
            case 1:
                return !(isalnum(addr_[0]) || addr_[0] == '_');
            case 2: {
                auto const w = *(u16 *) addr_;
                if (auto it = Uppercase2.find(w); it != Uppercase2.end())
                    return false;
                if (auto it = Uppercase3.find(w); it != Uppercase3.end())
                    return false;
                if (auto it = SmallLetters.find(w); it != SmallLetters.end())
                    return false;
                return true;
            }
        }
        return true;
    }
}

