#pragma once
#include <cstdint>
#include <string>

namespace ppx {
    using i8 = int8_t;
    using u8 = uint8_t;
    using u32 = uint32_t;

    class murmur {
    public:
        static u32 hash3(void const *, u32, u32);

    private:
        static u32 rotl32(u32 const x, i8 const r) {
            return (x << r) | (x >> (32 - r));
        }

        static u32 fmix32(u32 h) {
            h ^= h >> 16;
            h *= 0x85ebca6b;
            h ^= h >> 13;
            h *= 0xc2b2ae35;
            h ^= h >> 16;
            return h;
        }
    };
}
