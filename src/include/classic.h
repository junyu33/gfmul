#define __classic__
#include "common.h"
#define MOD_POLY 0x87ULL // 0b10000111

inline void gfmul(__int128 a, __int128 b, __int128* res) {
    __int128 result = 0;
    const __int128 mod = (__int128)MOD_POLY; // 模多项式

    for (int i = 0; i < 128; i++) {
        // 如果 b 的最低位是 1，累加 a
        if (b & 1) {
            result ^= a;
        }

        // 检查 a 的最高位是否需要模多项式归约
        int carry = (a >> 127) & 1;
        a <<= 1;
        if (carry) {
            a ^= mod; // 如果最高位溢出，用模多项式归约
        }

        // 右移 b，继续处理下一位
        b >>= 1;
    }
    *res = result;
}
#undef __classic__
