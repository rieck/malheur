/*
 * MurmurHash2, 64-bit versions, by Austin Appleby
 * --
 * The same caveats as 32-bit MurmurHash2 apply here - beware of alignment 
 * and endian-ness issues if used across multiple platforms.
 */

#include "config.h"
#include "common.h"

uint32_t MurmurHash2(const void *key, int32_t len, uint32_t seed)
{
    // 'm' and 'r' are mixing constants generated offline.
    // They're not really 'magic', they just happen to work well.

    const uint32_t m = 0x5bd1e995;
    const int32_t r = 24;

    // Initialize the hash to a 'random' value

    uint32_t h = seed ^ len;

    // Mix 4 bytes at a time into the hash

    const unsigned char *data = (const unsigned char *) key;

    while (len >= 4) {
        uint32_t k = *(uint32_t *) data;

        k *= m;
        k ^= k >> r;
        k *= m;

        h *= m;
        h ^= k;

        data += 4;
        len -= 4;
    }

    // Handle the last few bytes of the input array

    switch (len) {
    case 3:
        h ^= data[2] << 16;
    case 2:
        h ^= data[1] << 8;
    case 1:
        h ^= data[0];
        h *= m;
    };

    // Do a few final mixes of the hash to ensure the last few
    // bytes are well-incorporated.

    h ^= h >> 13;
    h *= m;
    h ^= h >> 15;

    return h;
}


/* 64-bit hash for 32-bit platforms */
uint64_t MurmurHash64B(const void *key, int32_t len, uint32_t seed)
{
    const uint32_t m = 0x5bd1e995;
    const int32_t r = 24;

    uint32_t h1 = seed ^ len;
    uint32_t h2 = 0;

    const uint32_t *data = (const uint32_t *) key;

    while (len >= 8) {
        uint32_t k1 = *data++;
        k1 *= m;
        k1 ^= k1 >> r;
        k1 *= m;
        h1 *= m;
        h1 ^= k1;
        len -= 4;

        uint32_t k2 = *data++;
        k2 *= m;
        k2 ^= k2 >> r;
        k2 *= m;
        h2 *= m;
        h2 ^= k2;
        len -= 4;
    }

    if (len >= 4) {
        uint32_t k1 = *data++;
        k1 *= m;
        k1 ^= k1 >> r;
        k1 *= m;
        h1 *= m;
        h1 ^= k1;
        len -= 4;
    }

    switch (len) {
    case 3:
        h2 ^= ((unsigned char *) data)[2] << 16;
    case 2:
        h2 ^= ((unsigned char *) data)[1] << 8;
    case 1:
        h2 ^= ((unsigned char *) data)[0];
        h2 *= m;
    };

    h1 ^= h2 >> 18;
    h1 *= m;
    h2 ^= h1 >> 22;
    h2 *= m;
    h1 ^= h2 >> 17;
    h1 *= m;
    h2 ^= h1 >> 19;
    h2 *= m;

    uint64_t h = h1;

    h = (h << 32) | h2;

    return h;
}
