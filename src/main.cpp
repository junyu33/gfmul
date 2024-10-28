#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "gfmul.h"

void print128(__m128i var) 
{
    int64_t v64val[2];
    memcpy(v64val, &var, sizeof(v64val));
    printf("%.16lx%.16lx", v64val[1], v64val[0]);
}


int main() {
    uint64_t mul1[2], mul2[2];
    scanf("%16lx%16lx", &mul1[1], &mul1[0]);
    scanf("%16lx%16lx", &mul2[1], &mul2[0]);

    // Define test cases
#ifdef __x86_64__
    // Test case for Intel architecture
    __m128i a = _mm_set_epi64x(mul1[1], mul1[0]);
    __m128i b = _mm_set_epi64x(mul2[1], mul2[0]);
#elif __aarch64__
    // Test case for ARM architecture
    uint64x2_t a = vcombine_u64(vld1_u64(mul1), vld1_u64(mul1 + 1));
    uint64x2_t b = vcombine_u64(vld1_u64(mul2), vld1_u64(mul2 + 1));
#elif __riscv
    // Test case for RISC-V architecture
    __int128 a = mul1[1];
    a = (a << 64) | mul1[0];
    __int128 b = mul2[1];
    b = (b << 64) | mul2[0];
#endif

    __m128i res;
    gfmul(a, b, &res);

    // Print results
    print128(res);

    return 0;
}
