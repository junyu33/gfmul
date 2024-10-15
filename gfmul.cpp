#include <stdio.h>
#include <string.h>
#include <stdint.h>
#ifdef __x86_64__
#include <immintrin.h>
#elif __aarch64__
#include <arm_neon.h>
#define __m128i uint8x16_t
#endif

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
inline void mul128_v2(__m128i a, __m128i b, __m128i *res1, __m128i *res2) {
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
    uint32x4_t XMMMASK = vdupq_n_u32(0xffffffff);
    mul128(a, b, &Tmp3, &Tmp6);
    tmp3 = vreinterpretq_u32_u8(Tmp3);
    tmp6 = vreinterpretq_u32_u8(Tmp6);
    tmp7 = vshrq_n_u32(tmp6, 31);
    tmp8 = vshrq_n_u32(tmp6, 30);
    tmp9 = vshrq_n_u32(tmp6, 25);
    tmp7 = veorq_u32(tmp7, tmp8);
    tmp7 = veorq_u32(tmp7, tmp9);
    tmp8 = vsetq_lane_u32(vgetq_lane_u32(tmp7, 3), tmp8, 0);  // 第一个元素来自 tmp7 的第3个元素
    tmp8 = vsetq_lane_u32(vgetq_lane_u32(tmp7, 0), tmp8, 1);  // 第二个元素来自 tmp7 的第0个元素
    tmp8 = vsetq_lane_u32(vgetq_lane_u32(tmp7, 2), tmp8, 2);  // 第三个元素来自 tmp7 的第2个元素
    tmp8 = vsetq_lane_u32(vgetq_lane_u32(tmp7, 1), tmp8, 3);  // 第四个元素来自 tmp7 的第1个元素


    tmp7 = vandq_u32(tmp8, XMMMASK);
    tmp8 = vbicq_u32(tmp8, XMMMASK);
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
#endif

void print128(__m128i var) 
{
    int64_t v64val[2];
    memcpy(v64val, &var, sizeof(v64val));
    printf("%.16llx%.16llx", v64val[1], v64val[0]);
}

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
    __m128i a = vreinterpretq_u8_u64(vcombine_u64(vld1_u64(mul1), vld1_u64(mul1 + 1)));
    __m128i b = vreinterpretq_u8_u64(vcombine_u64(vld1_u64(mul2), vld1_u64(mul2 + 1)));
#endif

    __m128i res;
    gfmul(a, b, &res);

    // Print results
    print128(res);

    return 0;
}
