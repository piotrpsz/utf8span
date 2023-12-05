#include "span.h"

namespace ppx::utf8 {
    // Rewind cursor to beginning of source.
    void span::
    begin() noexcept {
        cursor_ = addr_;
        idx_ = 0;
    }
    // Rewind cursor to after end of source.
    void span::
    end() noexcept {
        cursor_ = sentinel_;
        idx_ = 0;
    }

    // Fetch utf8-scope for code-point at index;
    // After success cursor is AFTER that code-point.
    scope span::
    operator[](uint const idx) noexcept {
        if (cursor_ >= sentinel_ || idx < idx_)
            begin();

        if (cursor_ < sentinel_ && idx >= idx_)
            if (skip(idx - idx_))
                return next();

        return {};
    }

    // Fetch multi-utf8-scope of a specific length.
    scope span::
    subspan(uint const idx, uint const len) noexcept {
        if (len == 0)
            return {};

        save();
        auto sc = operator[](idx);
        if (!sc.valid_chars()) {
            restore();
            return {};
        }

        save();
        for (uint i = 0; i < (len - 1); i++) {
            auto next_sc = next();
            if (!next_sc.valid_chars()) {
                restore();
                return {};
            }
            sc += next_sc;
        }
        restore();
        return sc;
    }

    // Fetch multi-utf8-scope to end of source.
    scope span::
    subspan(uint const idx) noexcept {
        save();
        auto sc = operator[](idx);
        if (!sc.valid_chars()) {
            restore();
            return {};
        }

        save();
        for (;;) {
            auto next_sc = next();
            if (!next_sc.valid_chars()) break;
            sc += next_sc;
        }
        restore();
        return sc;
    }

    // Get scope for current utf8.
    scope span::
    next() noexcept {
        if (cursor_ < sentinel_) {
            auto const len = codepoint_size(cursor_);
            if (cursor_ + len <= sentinel_) {
                scope rv(cursor_, len, 1);
                cursor_ += len;
                idx_ += 1;
                return rv;
            }
        }
        return {};
    }

    // Get scope for previous utf8.
    scope span::
    prev() noexcept {
        if (cursor_ > addr_ && cursor_ <= sentinel_) {
            if (cursor_ - 3 >= addr_) {
                if (codepoint_size(cursor_ - 3) == 3) {
                    cursor_ -= 3;
                    idx_ -= 1;
                    return {cursor_, 3, 1};
                }
            }
            if (cursor_ - 2 >= addr_) {
                if (codepoint_size(cursor_ - 2) == 2) {
                    cursor_ -= 2;
                    idx_ -= 1;
                    return {cursor_, 2, 1};
                }
            }
            if (cursor_ - 1 >= addr_) {
                cursor_ -= 1;
                idx_ -= 1;
                return {cursor_, 1, 1};
            }
        }
        return {};
    }

    std::optional<std::pair<uint,scope>> span::
    search(scope const& sc, int const start_pos) noexcept {
        static int start_idx{};  // indexes code-points
        if (start_pos == 0)
            start_idx = 0;
        if (start_idx > 0 && start_pos < start_idx)
            start_idx -= int(start_idx - start_pos);
        else
            start_idx = int(start_pos);

        if (sc.size_ == 0 || sc.len_ == 0)
            return {};

        for (int idx = start_idx;; idx++) {
            auto sub = subspan(idx, sc.len_);
            if (0 == sub.len_) break;
            if (sub.size_ == sc.size_) {
                if (0 == memcmp(sub.addr_, sc.addr_, sc.size_)) {
                    start_idx = idx + 1;
                    return std::make_pair(idx, sub);
                }
            }
        }
        return {};
    }

    bool span::
    skip(uint n) noexcept {
        if (size_ == 0)
            return false;
        if (n == 0)
            return true;

        save();
        for (uint i = 0; i < n; i++) {
            if (auto sc = next(); !sc.valid_chars()) {
                restore();
                return false;
            }
        }
        return true;
    }
}