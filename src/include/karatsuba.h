#define __karatsuba__ 
#include "common.h"
#define MOD_POLY 0x87
inline void gfmul(__int128 a, __int128 b, __int128* res) {
    // 分为高64位和低64位
    uint64_t a_high = (uint64_t)(a >> 64);
    uint64_t a_low = (uint64_t)a;
    uint64_t b_high = (uint64_t)(b >> 64);
    uint64_t b_low = (uint64_t)b;

    // z0 = a_low * b_low
    uint64_t z0 = 0;
    for (int i = 0; i < 64; i++) {
        if (b_low & ((uint64_t)1 << i)) {
            z0 ^= (a_low << i);
        }
    }

    // z2 = a_high * b_high
    uint64_t z2 = 0;
    for (int i = 0; i < 64; i++) {
        if (b_high & ((uint64_t)1 << i)) {
            z2 ^= (a_high << i);
        }
    }

    // z1 = (a_low ^ a_high) * (b_low ^ b_high)
    uint64_t a_mid = a_low ^ a_high;
    uint64_t b_mid = b_low ^ b_high;
    uint64_t z1 = 0;
    for (int i = 0; i < 64; i++) {
        if (b_mid & ((uint64_t)1 << i)) {
            z1 ^= (a_mid << i);
        }
    }

    // Karatsuba 组合
    __int128 result = ((__int128)z2 << 128) ^                    // 高位部分
                      ((__int128)(z1 ^ z0 ^ z2) << 64) ^        // 中间部分
                      z0;                                       // 低位部分

    // 模多项式归约
    for (int i = 127; i >= 0; i--) {
        if (result & ((__int128)1 << (128 + i))) {
            result ^= (__int128)MOD_POLY << i;
        }
    }

    *res = result;
}
#undef __karatsuba__
