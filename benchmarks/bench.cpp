#include <cstdint>
#include <chrono>
#include "../src/include/select.h"

const int iterations = 10000000;  // 10^7 次运算
__int128 a[iterations], b[iterations], res;

void init() {
    FILE* file = fopen("random_numbers.txt", "r");
    if (!file) {
        fprintf(stderr, "Error opening file\n");
        return;
    }

    uint64_t mul1[2], mul2[2];
    for (int i = 0; i < iterations; ++i) {
        if (fscanf(file, "%16lx%16lx", &mul1[1], &mul1[0]) != 2) {
            fprintf(stderr, "Error reading input data\n");
            fclose(file);
            return;
        }
        if (fscanf(file, "%16lx%16lx", &mul2[1], &mul2[0]) != 2) {
            fprintf(stderr, "Error reading input data\n");
            fclose(file);
            return;
        }
        a[i] = concat64(mul1[1], mul1[0]);
        b[i] = concat64(mul2[1], mul2[0]);
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

    return 0;
}
