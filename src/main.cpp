#include <cstdint>
#include <stdio.h>
#include <stdint.h>
#include "include/select.h"

int main() {
    uint64_t mul1[2], mul2[2];
    scanf("%16lx%16lx", &mul1[1], &mul1[0]);
    scanf("%16lx%16lx", &mul2[1], &mul2[0]);

    __int128 a = concat64(mul1[1], mul1[0]);
    __int128 b = concat64(mul2[1], mul2[0]);

    __int128 res;
    gfmul(a, b, &res);
    print128(res);
    return 0;
}
