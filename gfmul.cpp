#include <stdio.h>
#include <string.h>
#include <stdint.h>
#ifdef __x86_64__
#include <immintrin.h>
#elif __aarch64__
#include <arm_neon.h>
#define __m128i uint8x16_t
#elif __riscv
#include <riscv_vector.h>
#define __m128i __int128
#endif

void print128(__m128i var) 
{
    int64_t v64val[2];
    memcpy(v64val, &var, sizeof(v64val));
    printf("%.16llx%.16llx", v64val[1], v64val[0]);
}


/* multiplication in galois field without reduction */
#ifdef __x86_64__
__attribute__((target("sse2,pclmul")))
inline void mul128(__m128i a, __m128i b, __m128i *res1, __m128i *res2) {
    __m128i tmp3, tmp4, tmp5, tmp6;
    tmp3 = _mm_clmulepi64_si128(a, b, 0x00);
    tmp4 = _mm_clmulepi64_si128(a, b, 0x10);
    tmp5 = _mm_clmulepi64_si128(a, b, 0x01);
    tmp6 = _mm_clmulepi64_si128(a, b, 0x11);

    tmp4 = _mm_xor_si128(tmp4, tmp5);
    tmp5 = _mm_slli_si128(tmp4, 8);
    tmp4 = _mm_srli_si128(tmp4, 8);
    tmp3 = _mm_xor_si128(tmp3, tmp5);
    tmp6 = _mm_xor_si128(tmp6, tmp4);
    // initial mul now in tmp3, tmp6
    *res1 = tmp3;
    *res2 = tmp6;
}
#elif __aarch64__
inline void mul128(__m128i a, __m128i b, __m128i *res1, __m128i *res2) {
    __m128i tmp3, tmp4, tmp5, tmp6;
    poly64_t a_lo = (poly64_t)vgetq_lane_u64(vreinterpretq_u64_u8(a), 0);
    poly64_t b_lo = (poly64_t)vgetq_lane_u64(vreinterpretq_u64_u8(b), 0);
    poly64_t a_hi = (poly64_t)vgetq_lane_u64(vreinterpretq_u64_u8(a), 1);
    poly64_t b_hi = (poly64_t)vgetq_lane_u64(vreinterpretq_u64_u8(b), 1);

    tmp3 = vreinterpretq_u8_p128(vmull_p64(a_lo, b_lo));
    tmp4 = vreinterpretq_u8_p128(vmull_p64(a_hi, b_lo));
    tmp5 = vreinterpretq_u8_p128(vmull_p64(a_lo, b_hi));
    tmp6 = vreinterpretq_u8_p128(vmull_p64(a_hi, b_hi));

    tmp4 = veorq_u8(tmp4, tmp5);
    __m128i result, zero = vdupq_n_u8(0);
    tmp5 = vextq_u8(zero, tmp4, 8);
    tmp4 = vextq_u8(tmp4, zero, 8); // shift left by 8 bytes
    tmp3 = veorq_u8(tmp3, tmp5);
    tmp6 = veorq_u8(tmp6, tmp4);

    *res1 = tmp3;
    *res2 = tmp6;
}
#elif __riscv
inline void mul128(__m128i a, __m128i b, __m128i *res1, __m128i *res2) {
    // Split a and b into 64-bit lower and upper parts
    uint64_t a_lo = a, a_hi = a >> 64;
    uint64_t b_lo = b, b_hi = b >> 64;
    uint64_t tmp3_lo, tmp3_hi, tmp4_lo, tmp4_hi, 
             tmp5_lo, tmp5_hi, tmp6_lo, tmp6_hi;

    // Multiply lower parts, vclmul.vv
    __asm__ __volatile__ (
        "vsetvli t0, x0, e64, m1\n" 
        "vmv.v.x v8, %8\n"          
        "vmv.v.x v9, %9\n"
        "vmv.v.x v10, %10\n"
        "vmv.v.x v11, %11\n"

        "vclmul.vv v0, v8, v9\n"  // tmp3_low = a_low * b_low
        "vclmulh.vv v1, v8, v9\n" // tmp3_high = a_low * b_low
        "vclmul.vv v2, v10, v9\n" // tmp4_low = a_high * b_low
        "vclmulh.vv v3, v10, v9\n" // tmp4_high = a_high * b_low
        "vclmul.vv v4, v8, v11\n" // tmp5_low = a_low * b_high
        "vclmulh.vv v5, v8, v11\n" // tmp5_high = a_low * b_high
        "vclmul.vv v6, v10, v11\n" // tmp6_low = a_high * b_high
        "vclmulh.vv v7, v10, v11\n" // tmp6_high = a_high * b_high
        
        "vmv.x.s %0, v0\n"
        "vmv.x.s %1, v1\n"
        "vmv.x.s %2, v2\n"
        "vmv.x.s %3, v3\n"
        "vmv.x.s %4, v4\n"
        "vmv.x.s %5, v5\n"
        "vmv.x.s %6, v6\n"
        "vmv.x.s %7, v7\n"

        : "=r" (tmp3_lo), "=r" (tmp3_hi), 
          "=r" (tmp4_lo), "=r" (tmp4_hi), 
          "=r" (tmp5_lo), "=r" (tmp5_hi), 
          "=r" (tmp6_lo), "=r" (tmp6_hi)
        : "r" (a_lo), "r" (b_lo), "r" (a_hi), "r" (b_hi)
        : "t0", "v0", "v1", "v2", "v3", "v4", "v5", "v6", "v7", "v8", "v9", "v10", "v11"
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
#endif


/* multiplication in galois field with reduction */
#ifdef __x86_64__
__attribute__((target("sse2,pclmul")))
inline void gfmul (__m128i a, __m128i b, __m128i *res){
    __m128i tmp3, tmp6, tmp7, tmp8, tmp9, tmp10, tmp11, tmp12;
    __m128i XMMMASK = _mm_setr_epi32(0xffffffff, 0x0, 0x0, 0x0);
    mul128(a, b, &tmp3, &tmp6);
    tmp7 = _mm_srli_epi32(tmp6, 31);
    tmp8 = _mm_srli_epi32(tmp6, 30);
    tmp9 = _mm_srli_epi32(tmp6, 25);
    tmp7 = _mm_xor_si128(tmp7, tmp8);
    tmp7 = _mm_xor_si128(tmp7, tmp9);
    tmp8 = _mm_shuffle_epi32(tmp7, 147);

    tmp7 = _mm_and_si128(XMMMASK, tmp8);
    tmp8 = _mm_andnot_si128(XMMMASK, tmp8);
    tmp3 = _mm_xor_si128(tmp3, tmp8);
    tmp6 = _mm_xor_si128(tmp6, tmp7);
    tmp10 = _mm_slli_epi32(tmp6, 1);
    tmp3 = _mm_xor_si128(tmp3, tmp10);
    tmp11 = _mm_slli_epi32(tmp6, 2);
    tmp3 = _mm_xor_si128(tmp3, tmp11);
    tmp12 = _mm_slli_epi32(tmp6, 7);
    tmp3 = _mm_xor_si128(tmp3, tmp12);

    *res = _mm_xor_si128(tmp3, tmp6);
}
#elif __aarch64__
inline void gfmul(__m128i a, __m128i b, __m128i *res){
    uint32x4_t tmp3, tmp6, tmp7, tmp8, tmp9, tmp10, tmp11, tmp12;
    uint8x16_t Tmp3, Tmp6;
    uint32x4_t XMMMASK = {0xffffffff, 0x0, 0x0, 0x0};
    uint32x4_t INV_XMMMASK = {0x0, 0xffffffff, 0xffffffff, 0xffffffff};
    mul128(a, b, &Tmp3, &Tmp6);
    tmp3 = vreinterpretq_u32_u8(Tmp3);
    tmp6 = vreinterpretq_u32_u8(Tmp6);
    tmp7 = vshrq_n_u32(tmp6, 31);
    tmp8 = vshrq_n_u32(tmp6, 30);
    tmp9 = vshrq_n_u32(tmp6, 25);
    tmp7 = veorq_u32(tmp7, tmp8);
    tmp7 = veorq_u32(tmp7, tmp9);

    tmp8 = vsetq_lane_u32(vgetq_lane_u32(tmp7, 3), tmp8, 0);
    tmp8 = vsetq_lane_u32(vgetq_lane_u32(tmp7, 2), tmp8, 3);
    tmp8 = vsetq_lane_u32(vgetq_lane_u32(tmp7, 1), tmp8, 2);
    tmp8 = vsetq_lane_u32(vgetq_lane_u32(tmp7, 0), tmp8, 1);

    tmp7 = vandq_u32(tmp8, XMMMASK);
    tmp8 = vandq_u32(tmp8, INV_XMMMASK);
    tmp3 = veorq_u32(tmp3, tmp8);
    tmp6 = veorq_u32(tmp6, tmp7);
    tmp10 = vshlq_n_u32(tmp6, 1);
    tmp3 = veorq_u32(tmp3, tmp10);
    tmp11 = vshlq_n_u32(tmp6, 2);
    tmp3 = veorq_u32(tmp3, tmp11);
    tmp12 = vshlq_n_u32(tmp6, 7);
    tmp3 = veorq_u32(tmp3, tmp12);

    *res = vreinterpretq_u8_u32(veorq_u32(tmp3, tmp6));
}
#elif __riscv
void sll128_epi32(__m128i vec, uint32_t shift_amount, __m128i *result) {
    uint32_t *vec_ptr = (uint32_t *)&vec;
    uint32_t *res_ptr = (uint32_t *)result;

    __asm__ __volatile__ (
        "vsetvli t0, x0, e32, m1\n"          // Set vector length to 32-bit elements (4 elements)
        "vle32.v v1, (%1)\n"                 // Load vec into v1 (4x32-bit elements)
        "vsll.vx v2, v1, %2\n"               // Perform shift left logical on v1 by shift_amount
        "vse32.v v2, (%0)\n"                 // Store result from v2 into memory (res_ptr)
        :                                   // No outputs
        : "r"(res_ptr), "r"(vec_ptr), "r"(shift_amount)
        : "t0", "v1", "v2", "memory"
    );
}
void srl128_epi32(__m128i vec, uint32_t shift_amount, __m128i *result) {
    uint32_t *vec_ptr = (uint32_t *)&vec;
    uint32_t *res_ptr = (uint32_t *)result;

    __asm__ __volatile__ (
        "vsetvli t0, x0, e32, m1\n"          // Set vector length to 32-bit elements (4 elements)
        "vle32.v v1, (%1)\n"                 // Load vec into v1 (4x32-bit elements)
        "vsrl.vx v2, v1, %2\n"               // Perform shift right logical on v1 by shift_amount
        "vse32.v v2, (%0)\n"                 // Store result from v2 into memory (res_ptr)
        :                                   // No outputs
        : "r"(res_ptr), "r"(vec_ptr), "r"(shift_amount)
        : "t0", "v1", "v2", "memory"
    );
}
inline void gfmul(__m128i a, __m128i b, __m128i *res){
    __m128i tmp3, tmp6, tmp7, tmp8, tmp9, tmp10, tmp11, tmp12;
    mul128(a, b, &tmp3, &tmp6);
    srl128_epi32(tmp6, 31, &tmp7);
    srl128_epi32(tmp6, 30, &tmp8);
    srl128_epi32(tmp6, 25, &tmp9);
    tmp7 = tmp7 ^ tmp8;
    tmp7 = tmp7 ^ tmp9;
    tmp8 = ((__m128i)(tmp7 >> 96) & 0xffffffff) << 0 | 
           ((__m128i)(tmp7 >> 64) & 0xffffffff) << 96 |
           ((__m128i)(tmp7 >> 32) & 0xffffffff) << 64 |
           ((__m128i)(tmp7) & 0xffffffff) << 32;
                    

    tmp7 = tmp8 & 0xffffffff;
    tmp8 = (tmp8 >> 32) << 32;
    tmp3 = tmp3 ^ tmp8;
    tmp6 = tmp6 ^ tmp7;
    sll128_epi32(tmp6, 1, &tmp10);
    tmp3 = tmp3 ^ tmp10;
    sll128_epi32(tmp6, 2, &tmp11);
    tmp3 = tmp3 ^ tmp11;
    sll128_epi32(tmp6, 7, &tmp12);
    tmp3 = tmp3 ^ tmp12;

    *res = tmp3 ^ tmp6;
}
#endif

int main() {
    uint64_t mul1[2], mul2[2];
    scanf("%16llx%16llx", &mul1[1], &mul1[0]);
    scanf("%16llx%16llx", &mul2[1], &mul2[0]);

    // Define test cases
#ifdef __x86_64__
    // Test case for Intel architecture
    __m128i a = _mm_set_epi64x(mul1[1], mul1[0]);
    __m128i b = _mm_set_epi64x(mul2[1], mul2[0]);
#elif __aarch64__
    // Test case for ARM architecture
    uint8x16_t a = vreinterpretq_u8_u64(vcombine_u64(vld1_u64(mul1), vld1_u64(mul1 + 1)));
    uint8x16_t b = vreinterpretq_u8_u64(vcombine_u64(vld1_u64(mul2), vld1_u64(mul2 + 1)));
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

/*
 * inline void mul128_v2(__m128i a, __m128i b, __m128i *res1, __m128i *res2) {
    uint32x4_t tmp3, tmp4, tmp5, tmp6;
    poly64_t a_lo = (poly64_t)vgetq_lane_u64(vreinterpretq_u64_u8(a), 0);
    poly64_t b_lo = (poly64_t)vgetq_lane_u64(vreinterpretq_u64_u8(b), 0);
    poly64_t a_hi = (poly64_t)vgetq_lane_u64(vreinterpretq_u64_u8(a), 1);
    poly64_t b_hi = (poly64_t)vgetq_lane_u64(vreinterpretq_u64_u8(b), 1);

    tmp3 = vreinterpretq_u32_p128(vmull_p64(a_lo, b_lo));
    tmp4 = vreinterpretq_u32_p128(vmull_p64(a_hi, b_lo));
    tmp5 = vreinterpretq_u32_p128(vmull_p64(a_lo, b_hi));
    tmp6 = vreinterpretq_u32_p128(vmull_p64(a_hi, b_hi));

    tmp4 = veorq_u32(tmp4, tmp5);
    uint32x4_t zero = vdupq_n_u32(0);
    tmp5 = vextq_u32(zero, tmp4, 2);
    tmp4 = vextq_u32(tmp4, zero, 2); // shift left by 8 bytes
    tmp3 = veorq_u32(tmp3, tmp5);
    tmp6 = veorq_u32(tmp6, tmp4);

    *res1 = vreinterpretq_u8_u32(tmp3);
    *res2 = vreinterpretq_u8_u32(tmp6);
}
*/
