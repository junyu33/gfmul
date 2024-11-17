#ifndef COMMON_H
#define COMMON_H
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#ifdef __ntl__
#define __classic__
#endif

#ifndef __classic__
#ifdef __x86_64__
#include <immintrin.h>
#define __int128 __m128i
#elif __aarch64__
#include <arm_neon.h>
#define __int128 uint64x2_t
#elif __riscv
#include <riscv_vector.h>
#endif
#endif

inline __int128 concat64(uint64_t a, uint64_t b) {
#ifdef __classic__
    return (__int128)a << 64 | b;
#elif __x86_64__
    return (__int128)_mm_set_epi64x(a, b);
#elif __aarch64__
    return (__int128)vcombine_u64(vcreate_u64(b), vcreate_u64(a));
#elif __riscv
    return (__int128) a << 64 | b;
#endif
}

inline void print128(__int128 var) 
{
    int64_t v64val[2];
    memcpy(v64val, &var, sizeof(v64val));
    printf("%.16lx%.16lx", v64val[1], v64val[0]);
}

inline void print64(uint64_t var) 
{
    printf("%.16lx", var);
}

#endif
