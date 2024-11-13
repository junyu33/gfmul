#ifndef GFMUL_H
#define GFMUL_H

#ifdef __x86_64__
#include <immintrin.h>
#elif __aarch64__
#include <arm_neon.h>
#define __m128i uint64x2_t
#elif __riscv
#include <riscv_vector.h>
#define __m128i __int128
#endif

/* multiplication in galois field with reduction */
#ifdef __x86_64__
__attribute__((target("sse2,pclmul")))
inline void gfmul64(__m128i a, __m128i b, u_int64_t *res) {
    __m128i mul;
    u_int64_t tmp3, tmp6, tmp7, tmp8, tmp9, tmp10, tmp11, tmp12;

    mul = _mm_clmulepi64_si128(a, b, 0x00);
    tmp3 = _mm_extract_epi64(mul, 0);
    tmp6 = _mm_extract_epi64(mul, 1);

    tmp7 = tmp6 >> 63;
    tmp8 = tmp6 >> 61;
    tmp9 = tmp6 >> 60;
    tmp7 = tmp7 ^ tmp8;
    tmp7 = tmp7 ^ tmp9;

    tmp6 = tmp6 ^ tmp7;
    tmp10 = tmp6 << 1;
    tmp11 = tmp6 << 3;
    tmp12 = tmp6 << 4;
    tmp3 = tmp3 ^ tmp10;
    tmp3 = tmp3 ^ tmp11;
    tmp3 = tmp3 ^ tmp12;

    *res = tmp3 ^ tmp6;
}
inline void gfmul (__m128i a, __m128i b, __m128i *res){
    __m128i tmp3, tmp4, tmp5, tmp6, tmp7, tmp8, tmp9, tmp10, tmp11, tmp12;
    __m128i XMMMASK = _mm_setr_epi32(0xffffffff, 0xffffffff, 0x0, 0x0);
    tmp3 = _mm_clmulepi64_si128(a, b, 0x00);
    tmp4 = _mm_clmulepi64_si128(a, b, 0x10);
    tmp5 = _mm_clmulepi64_si128(a, b, 0x01);
    tmp6 = _mm_clmulepi64_si128(a, b, 0x11);

    tmp4 = _mm_xor_si128(tmp4, tmp5);
    tmp5 = _mm_slli_si128(tmp4, 8);
    tmp4 = _mm_srli_si128(tmp4, 8);
    tmp3 = _mm_xor_si128(tmp3, tmp5);
    tmp6 = _mm_xor_si128(tmp6, tmp4);
    tmp7 = _mm_srli_epi64(tmp6, 63);
    tmp8 = _mm_srli_epi64(tmp6, 62);
    tmp9 = _mm_srli_epi64(tmp6, 57);
    tmp7 = _mm_xor_si128(tmp7, tmp8);
    tmp7 = _mm_xor_si128(tmp7, tmp9);
    tmp8 = _mm_shuffle_epi32(tmp7, 0x4e);

    tmp7 = _mm_and_si128(XMMMASK, tmp8);
    tmp8 = _mm_andnot_si128(XMMMASK, tmp8);
    tmp3 = _mm_xor_si128(tmp3, tmp8);
    tmp6 = _mm_xor_si128(tmp6, tmp7);
    tmp10 = _mm_slli_epi64(tmp6, 1);
    tmp11 = _mm_slli_epi64(tmp6, 2);
    tmp12 = _mm_slli_epi64(tmp6, 7);
    tmp3 = _mm_xor_si128(tmp3, tmp10);
    tmp3 = _mm_xor_si128(tmp3, tmp11);
    tmp3 = _mm_xor_si128(tmp3, tmp12);

    *res = _mm_xor_si128(tmp3, tmp6);
}
#elif __aarch64__
inline void gfmul64(__m128i a, __m128i b, uint64_t *res) {
    poly128_t mul; // 128-bit result from carry-less multiplication
    uint64_t tmp3, tmp6, tmp7, tmp8, tmp9, tmp10, tmp11, tmp12;

    // Perform carry-less multiplication
    mul = vmull_p64(vgetq_lane_u64(a, 0), vgetq_lane_u64(b, 0));
    tmp3 = (uint64_t)(mul);           
    tmp6 = (uint64_t)(mul >> 64);     

    // Perform the same bitwise operations as in the Intel version
    tmp7 = tmp6 >> 63;
    tmp8 = tmp6 >> 61;
    tmp9 = tmp6 >> 60;
    tmp7 = tmp7 ^ tmp8;
    tmp7 = tmp7 ^ tmp9;

    tmp6 = tmp6 ^ tmp7;
    tmp10 = tmp6 << 1;
    tmp11 = tmp6 << 3;
    tmp12 = tmp6 << 4;
    tmp3 = tmp3 ^ tmp10;
    tmp3 = tmp3 ^ tmp11;
    tmp3 = tmp3 ^ tmp12;

    *res = tmp3 ^ tmp6;
}
inline void gfmul(__m128i a, __m128i b, __m128i *res){
    uint64x2_t tmp3, tmp4, tmp5, tmp6, tmp7, tmp8, tmp9, tmp10, tmp11, tmp12;
    uint64x2_t XMMMASK = {0xffffffffffffffff, 0x0};
    uint64x2_t INV_XMMMASK = {0x0, 0xffffffffffffffff};
    poly64_t a_lo = (poly64_t)vgetq_lane_u64(a, 0);
    poly64_t b_lo = (poly64_t)vgetq_lane_u64(b, 0);
    poly64_t a_hi = (poly64_t)vgetq_lane_u64(a, 1);
    poly64_t b_hi = (poly64_t)vgetq_lane_u64(b, 1);

    tmp3 = vreinterpretq_u64_p128(vmull_p64(a_lo, b_lo));
    tmp4 = vreinterpretq_u64_p128(vmull_p64(a_hi, b_lo));
    tmp5 = vreinterpretq_u64_p128(vmull_p64(a_lo, b_hi));
    tmp6 = vreinterpretq_u64_p128(vmull_p64(a_hi, b_hi));

    tmp4 = veorq_u64(tmp4, tmp5);
    __m128i zero = vdupq_n_u64(0);
    tmp5 = vextq_u64(zero, tmp4, 1);
    tmp4 = vextq_u64(tmp4, zero, 1); 
    tmp3 = veorq_u64(tmp3, tmp5);
    tmp6 = veorq_u64(tmp6, tmp4);
    tmp7 = vshrq_n_u64(tmp6, 63);
    tmp8 = vshrq_n_u64(tmp6, 62);
    tmp9 = vshrq_n_u64(tmp6, 57);
    tmp7 = veorq_u64(tmp7, tmp8);
    tmp7 = veorq_u64(tmp7, tmp9);
    tmp8 = vextq_u64(tmp7, tmp7, 1);

    tmp7 = vandq_u64(tmp8, XMMMASK);
    tmp8 = vandq_u64(tmp8, INV_XMMMASK);
    tmp3 = veorq_u64(tmp3, tmp8);
    tmp6 = veorq_u64(tmp6, tmp7);
    tmp10 = vshlq_n_u64(tmp6, 1);
    tmp11 = vshlq_n_u64(tmp6, 2);
    tmp12 = vshlq_n_u64(tmp6, 7);
    tmp3 = veorq_u64(tmp3, tmp10);
    tmp3 = veorq_u64(tmp3, tmp11);
    tmp3 = veorq_u64(tmp3, tmp12);

    *res = veorq_u64(tmp3, tmp6);
}
#elif __riscv
inline void mul128(__m128i a, __m128i b, __m128i *res1, __m128i *res2) {
    // Split a and b into 64-bit lower and upper parts
    uint64_t a_lo = a, a_hi = a >> 64;
    uint64_t b_lo = b, b_hi = b >> 64;
    uint64_t tmp3_lo, tmp3_hi, tmp4_lo, tmp4_hi, 
             tmp5_lo, tmp5_hi, tmp6_lo, tmp6_hi;

    __asm__ __volatile__ (
        "mv t0, %8\n"             
        "mv t1, %9\n"             
        "mv t2, %10\n"             
        "mv t3, %11\n"             

        "clmul   %0, t0, t1\n"   
        "clmulh  %1, t0, t1\n" 
        "clmul   %2, t2, t1\n" 
        "clmulh  %3, t2, t1\n" 
        "clmul   %4, t0, t3\n" 
        "clmulh  %5, t0, t3\n" 
        "clmul   %6, t2, t3\n" 
        "clmulh  %7, t2, t3\n" 

        : "=r" (tmp3_lo), "=r" (tmp3_hi), 
          "=r" (tmp4_lo), "=r" (tmp4_hi), 
          "=r" (tmp5_lo), "=r" (tmp5_hi), 
          "=r" (tmp6_lo), "=r" (tmp6_hi)
        : "r" (a_lo), "r" (b_lo), "r" (a_hi), "r" (b_hi)
        : "t0", "t1", "t2", "t3"
    );

    __m128i tmp3 = ((__m128i)tmp3_hi << 64) | tmp3_lo;
    __m128i tmp4 = ((__m128i)tmp4_hi << 64) | tmp4_lo;
    __m128i tmp5 = ((__m128i)tmp5_hi << 64) | tmp5_lo;
    __m128i tmp6 = ((__m128i)tmp6_hi << 64) | tmp6_lo;

    tmp4 = tmp4 ^ tmp5;
    tmp5 = tmp4 << 64;
    tmp4 = tmp4 >> 64;
    tmp3 = tmp3 ^ tmp5;
    tmp6 = tmp6 ^ tmp4;

    *res1 = tmp3;
    *res2 = tmp6;
}
inline void sll128_epi64(__m128i vec, uint64_t shift_amount, __m128i *result) {
    uint64_t *vec_ptr = (uint64_t *)&vec;
    uint64_t *res_ptr = (uint64_t *)result;

    __asm__ __volatile__ (
        "vsetvli t0, x0, e64, m1\n"          
        "vle64.v v1, (%1)\n"                 
        "vsll.vx v2, v1, %2\n"               
        "vse64.v v2, (%0)\n"                 
        :                                  
        : "r"(res_ptr), "r"(vec_ptr), "r"(shift_amount)
        : "t0", "v1", "v2", "memory"
    );
}
inline void srl128_epi64(__m128i vec, uint64_t shift_amount, __m128i *result) {
    uint64_t *vec_ptr = (uint64_t *)&vec;
    uint64_t *res_ptr = (uint64_t *)result;

    __asm__ __volatile__ (
        "vsetvli t0, x0, e64, m1\n"        
        "vle64.v v1, (%1)\n"                 
        "vsrl.vx v2, v1, %2\n"               
        "vse64.v v2, (%0)\n"                 
        :                                   
        : "r"(res_ptr), "r"(vec_ptr), "r"(shift_amount)
        : "t0", "v1", "v2", "memory"
    );
}
inline void gfmul(__m128i a, __m128i b, __m128i *res){
    __m128i tmp3, tmp6, tmp7, tmp8, tmp9, tmp10, tmp11, tmp12;
    mul128(a, b, &tmp3, &tmp6);
    srl128_epi64(tmp6, 63, &tmp7);
    srl128_epi64(tmp6, 62, &tmp8);
    srl128_epi64(tmp6, 57, &tmp9);
    tmp7 = tmp7 ^ tmp8;
    tmp7 = tmp7 ^ tmp9;
    tmp8 = (tmp7 >> 64) ^ (tmp7 << 64);

    tmp7 = tmp8 & 0xffffffffffffffff;
    tmp8 = (tmp8 >> 64) << 64;
    tmp3 = tmp3 ^ tmp8;
    tmp6 = tmp6 ^ tmp7;
    sll128_epi64(tmp6, 1, &tmp10);
    tmp3 = tmp3 ^ tmp10;
    sll128_epi64(tmp6, 2, &tmp11);
    tmp3 = tmp3 ^ tmp11;
    sll128_epi64(tmp6, 7, &tmp12);
    tmp3 = tmp3 ^ tmp12;

    *res = tmp3 ^ tmp6;
}
#endif
#endif


