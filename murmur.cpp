#include "murmur.h"

namespace ppx {
    u32 murmur::hash3(void const *const key, u32 const len, u32 const seed) {
        auto const data = static_cast<u8 const *>(key);
        auto const nblocks = len / 4;
        u32 h1 = seed;

        u32 const c1 = 0xcc9e2d51;
        u32 const c2 = 0x1b873593;

        auto const blocks = reinterpret_cast<u32 const *>(data + 4 * nblocks);
        for (int i = -nblocks; i; i++) {
            u32 k1 = blocks[i];
            k1 *= c1;
            k1 = rotl32(k1, 15);
            k1 *= c2;

            h1 ^= k1;
            h1 = rotl32(h1, 13);
            h1 = h1 * 5 + 0xe6546b64;
        }

        u8 const *const tail = data + nblocks * 4;
        u32 k1 = 0;
        switch (len & 3) {
            case 3:
                k1 ^= tail[2] << 16;
            case 2:
                k1 ^= tail[1] << 8;
            case 1:
                k1 ^= tail[0];
                k1 *= c1;
                k1 = rotl32(k1, 15);
                k1 *= c2;
                h1 ^= k1;
        };

        h1 ^= len;
        h1 = fmix32(h1);
        return h1;
    }
}

