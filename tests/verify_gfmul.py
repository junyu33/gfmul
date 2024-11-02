def sll8(x, n):
    return x << n & 0xff

def sll32(x, n):
    return x << n & 0xffffffff

def mul8(a: int, b: int):
    # 确保输入是8位的无符号整数
    mask_8bit = (1 << 8) - 1

    # 逐位进行非进位乘法，使用 & 操作执行非进位乘法
    result = 0
    for i in range(8):
        # 检查 b 的当前位是否为 1，如果是，执行与 a 的位与操作
        if (b >> i) & 1:
            result ^= (a << i)

    # 提取低8位
    low_8 = result & mask_8bit

    # 提取高8位
    high_8 = result >> 8

    return low_8, high_8

def mul32(a: int, b: int):
    # 确保输入是32位的无符号整数
    mask_32bit = (1 << 32) - 1

    # 逐位进行非进位乘法，使用 & 操作执行非进位乘法
    result = 0
    for i in range(32):
        # 检查 b 的当前位是否为 1，如果是，执行与 a 的位与操作
        if (b >> i) & 1:
            result ^= (a << i)

    # 提取低32位
    low_32 = result & mask_32bit

    # 提取高32位
    high_32 = result >> 32

    return low_32, high_32

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

def gfmul8(a, b):
    tmp3, tmp6 = mul8(a, b)
    tmp7 = ((tmp6 >> 7) ^ (tmp6 >> 5) ^ (tmp6 >> 4))
    tmp6 = tmp6 ^ tmp7
    tmp10 = sll8(tmp6, 1)
    tmp11 = sll8(tmp6, 3)
    tmp12 = sll8(tmp6, 4)
    
    tmp6 = tmp6 ^ tmp10
    tmp6 = tmp6 ^ tmp11
    tmp6 = tmp6 ^ tmp12
    return tmp3 ^ tmp6

def gfmul32(a, b):
    tmp3, tmp6 = mul32(a, b)
    print(hex(tmp3), hex(tmp6))
    tmp7 = ((tmp6 >> 30) ^ (tmp6 >> 29) ^ (tmp6 >> 25))
    tmp6 = tmp6 ^ tmp7
    tmp10 = sll32(tmp6, 2)
    tmp11 = sll32(tmp6, 3)
    tmp12 = sll32(tmp6, 7)
    
    tmp6 = tmp6 ^ tmp10
    tmp6 = tmp6 ^ tmp11
    tmp6 = tmp6 ^ tmp12
    return tmp3 ^ tmp6

a, b = 0x8512a248, 0xd550d894
print(hex(gfmul32(a, b)))

