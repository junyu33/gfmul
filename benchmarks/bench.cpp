#include <stdio.h>
#include <stdint.h>
#include <random>
#include <algorithm>
#include <chrono>
#include "../src/include/gfmul.h"
//#include "../src/include/wangxiao.h"
//#include "../src/include/intel.h"

const int iterations = 10000000;  // 10^6 次运算
__m128i a[iterations], b[iterations], res;

void init() {
    FILE* file = fopen("random_128bit_numbers.txt", "r");
    if (!file) {
        fprintf(stderr, "Error opening file\n");
        return;
    }

    uint64_t mul1[2], mul2[2];
    for (int i = 0; i < iterations; ++i) {
        if (fscanf(file, "%16lx%16lx", &mul1[1], &mul1[0]) != 2 ||
            fscanf(file, "%16lx%16lx", &mul2[1], &mul2[0]) != 2) {
            fprintf(stderr, "Error reading input data\n");
            fclose(file);
            return;
        }
#ifdef __x86_64__
        a[i] = _mm_set_epi64x(mul1[1], mul1[0]);
        b[i] = _mm_set_epi64x(mul2[1], mul2[0]);
#elif __aarch64__
        a[i] = vcombine_u64(vld1_u64(mul1), vld1_u64(mul1 + 1));
        b[i] = vcombine_u64(vld1_u64(mul2), vld1_u64(mul2 + 1));
#elif __riscv
        a[i] = (__int128)mul1[1] << 64 | mul1[0];
        b[i] = (__int128)mul2[1] << 64 | mul2[0];
#endif
    }
}

int main() {
    init();
    // 开始计时
    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < iterations; ++i) {
        gfmul(a[i], b[i], &res);
    }

    // 结束计时
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    printf("Total execution time: %.5f seconds\n", duration.count());

/*    std::random_device rd;*/
    /*std::mt19937 gen(rd());*/

    /*// 打乱数组 a 和 b*/
    /*std::shuffle(std::begin(a), std::end(a), gen);*/
    /*std::shuffle(std::begin(b), std::end(b), gen);*/

    /*start = std::chrono::high_resolution_clock::now();*/

    /*for (int i = 0; i < iterations; ++i) {*/
        /*gfmul_intel(a[i], b[i], &res);*/
    /*}*/

    /*// 结束计时*/
    /*end = std::chrono::high_resolution_clock::now();*/
    /*duration = end - start;*/
    /*printf("Total execution time: %.5f seconds\n", duration.count());*/
    return 0;
}
