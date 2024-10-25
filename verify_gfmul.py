def mul128(a: int, b: int):
    # 确保输入是128位的无符号整数
    mask_128bit = (1 << 128) - 1

    # 逐位进行非进位乘法，使用 & 操作执行非进位乘法
    result = 0
    for i in range(128):
        # 检查 b 的当前位是否为 1，如果是，执行与 a 的位与操作
        if (b >> i) & 1:
            result ^= (a << i)

    # 提取低128位
    low_128 = result & mask_128bit

    # 提取高128位
    high_128 = result >> 128

    return low_128, high_128  
def gfmul(a, b):
    tmp3, tmp6 = mul128(a, b)
    T = ((tmp6 >> 31) ^ (tmp6 >> 30) ^ (tmp6 >> 25))

    res = tmp3 ^ \
      (T >> 96) ^ tmp6 ^ \
      (((T >> 96) ^ tmp6) << 1) ^ \
      (((T >> 96) ^ tmp6) << 2) ^ \
      (((T >> 96) ^ tmp6) << 7)
    return res % (2 ** 128 - 1)

a, b = 98195696920426533817649554218743231561, 43027262476631949179376797970948942433
print(gfmul(a, b))
