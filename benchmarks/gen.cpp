#include <cstdio>
#include <cstdlib>
#include <random>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <bit_count (8-128)>\n", argv[0]);
        return 1;
    }

    // 获取命令行参数指定的比特数
    int bit_count = std::atoi(argv[1]);
    if (bit_count < 8 || bit_count > 128) {
        fprintf(stderr, "Error: bit count must be between 8 and 128\n");
        return 1;
    }

    const int num_random_numbers = 20000000;  // 生成的随机数数量
    FILE* file = fopen("random_numbers.txt", "w");
    if (!file) {
        fprintf(stderr, "Error opening file\n");
        return 1;
    }

    // 设置随机数生成器
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<uint64_t> dist(0, UINT64_MAX);

    for (int i = 0; i < num_random_numbers; ++i) {
        if (bit_count > 64) {
            // 生成128位随机数，通过两个64位拼接
            uint64_t high = (bit_count == 128) ? dist(gen) : dist(gen) & ((1ULL << (bit_count - 64)) - 1);
            uint64_t low = dist(gen);
            fprintf(file, "%lx%016lx\n", high, low);
        } else {
            // 生成64位或更小的随机数并使用掩码
            uint64_t num = (bit_count == 64) ? dist(gen) : dist(gen) & ((1ULL << (bit_count - 64)) - 1);
            fprintf(file, "%lx\n", num);
        }
    }

    fclose(file);
    printf("Random numbers generated in random_numbers.txt\n");
    return 0;
}
