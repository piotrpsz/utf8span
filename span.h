#pragma once

#include "scope.h"
#include "murmur.h"
#include <string>
#include <vector>
#include <optional>

namespace ppx::utf8 {

    class span : public scope {
        char const *cursor_{};
        char const *sentinel_;  // address of char after last (end of source buffer)
        uint idx_{};            // code-point index
        uint saved_idx_{};
        char const *saved_cursor_{};
        std::string store_{};
    public:
        span() = delete;

        ~span() override = default;

        explicit span(std::string const& str)
                : span(str.c_str(), str.size()) {
            store_ = str;
        }

        span(char const *const ptr, uint const size)
                : scope(ptr, int(size)), cursor_{ptr}, sentinel_{ptr + size} {
        }

        /// Rewind cursor to beginning of source.
        void begin() noexcept;

        /// Rewind cursor to after end of source.
        void end() noexcept;

        /// Fetch utf8-scope for code-point at index;
        /// After success cursor is AFTER that code-point.
        scope operator[](uint idx) noexcept;

        /// Returns whole span as scope
        scope operator()() noexcept {
            return subspan(0);
        }

        /// Fetch multi-utf8-scope of a specific length.
        /// \param idx - index of first code-point,
        /// \param len - number of expected code-points.
        /// \return scope for all code-points (if found).
        scope subspan(uint idx, uint len) noexcept;

        /// Fetch multi-utf8-scope to end of source.
        /// \param idx - index of first code-point.
        /// \return scope for all code-points (if found).
        scope subspan(uint idx) noexcept;

        /// Get scope for current utf8.
        /// After success cursor is moved forward (to the next utf8).
        scope next() noexcept;

        /// Get scope for previous utf8.
        /// After success cursor in moved to the utf8's position.
        scope prev() noexcept;

        /// Searches passed scope in the entire buffer.
        [[nodiscard]] std::optional<std::pair<uint, scope>>
        search(scope const& sc, int start_pos = 0) noexcept;

        /// Skips passed number of code-points. \n
        /// If something goes wrong the span remains unchanged.
        /// \param n - number code-points to skip.
        /// \return True if the operation was successful, False otherwise.
        /// \remark when span is empty returns always false and \n
        ///         true when n == 0 (when span is not empty).
        bool skip(uint n) noexcept;

    private:
        /// Saves current state of span.
        void save() noexcept {
            saved_idx_ = idx_;
            saved_cursor_ = cursor_;
        }

        /// Restores saved state of span.
        void restore() noexcept {
            idx_ = saved_idx_;
            cursor_ = saved_cursor_;
        }
    };
}

