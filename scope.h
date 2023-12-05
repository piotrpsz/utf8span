#pragma once
#include "murmur.h"
#include <cstdint>
#include <vector>
#include <unordered_map>
#include <cctype>

namespace ppx::utf8 {
    using u8 = uint8_t;
    using u16 = uint16_t;
    using u32 = uint32_t;
    using uint = unsigned int;
    using i64 = int64_t;

    class scope {
        friend class span;
    protected:
        // Constants (polish and german utf8 code-points).
        static const std::unordered_map<u16, std::vector<u8>> Uppercase2;
        static const std::unordered_map<u32, std::vector<u8>> Uppercase3;
        static const std::unordered_map<u16, std::vector<u8>> SmallLetters;
    protected:
        char const *addr_{};
        int size_{};    // number of bytes
        int len_{0};    // number of code-points
    public:
        scope() = default;
        virtual ~scope() = default;
        scope(scope const&) = default;
        scope& operator=(scope const&) = default;
        scope(scope&&) = default;
        scope& operator=(scope&&) = default;

        scope(char const *const ptr, int const size)
                : addr_{ptr}, size_{size} {
        }

        scope(char const *ptr, int const size, int const len)
                : addr_{ptr}, size_{size}, len_{len} {
        }

        /// Returns number of bytes.
        [[nodiscard]] int size() const noexcept {
            return size_;
        }

        /// Returns number of code-points
        [[nodiscard]] int len() const noexcept {
            return len_;
        }

        /// Adds two scopes. The added scopes must create continuity.
        scope& operator+=(scope const& rhs) noexcept;

        /// Checks if contains any valid code-point
        [[nodiscard]] bool valid_chars() const noexcept {
            return len_ > 0;
        }

        /// Checks if contains any byte.
        [[nodiscard]] bool empty() const noexcept {
            return size_ == 0;
        }

        /// Returs pair: address of first byte and number of bytes in scope.
        std::pair<char const *, int> operator()() const noexcept {
            return {addr_, size_};
        }

        /// Converts to std::string.
        [[nodiscard]] std::string str() const noexcept {
            return {addr_, unsigned(size_)};
        }

        /// Computes hash for specified scope.
        [[nodiscard]] u32 hash() const noexcept;

        /// Converts all code-point to it uppercase version (if needed).
        [[nodiscard]] std::vector<char> to_upper() const noexcept;

        /// Converts all code-points to it small versions (if needed).
        [[nodiscard]] std::vector<char> to_lower() const noexcept;

    protected:
        /// Checks if code-point is word boundary
        [[nodiscard]] bool is_word_boundary() const noexcept;

        /// Checks how many bytes contains pointed code-point.
        static uint codepoint_size(char const *ptr) noexcept;
    };
}
