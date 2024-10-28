#include <cstdio>
#include <random>

int main() {
    const int num_random_numbers = 20000000;  // 生成的随机数数量
    FILE* file = fopen("random_128bit_numbers.txt", "w");
    if (!file) {
        fprintf(stderr, "Error opening file\n");
        return 1;
    }

    // 设置随机数生成器
    std::random_device rd;
    std::mt19937_64 gen(rd());  // 使用64位的Mersenne Twister生成器
    std::uniform_int_distribution<uint64_t> dist(0, UINT64_MAX);

    for (int i = 0; i < num_random_numbers; ++i) {
        // 生成128位的随机数，通过两个64位拼接
        uint64_t high = dist(gen);
        uint64_t low = dist(gen);

        // 使用 printf 输出到文件，格式化为 16 进制
        fprintf(file, "%016llx%016llx\n", high, low);
    }

    fclose(file);
    printf("Random numbers generated in random_128bit_numbers.txt\n");
    return 0;
}
