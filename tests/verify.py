def no_carry_multiply(a: int, b: int) -> int:
    result = 0
    for i in range(128):
        if (b >> i) & 1:  # 检查 b 的第 i 位
            result ^= (a << i)  # 异或相加，不进位
    return result

# 打印256位整数的hex值
def print_hex256(num: int):
    # 将整数转换为16进制并去掉 '0x' 前缀，填充到64位（256位/4）长度
    print(f"{num:064x}")

# 示例值
a = int(input(), 16)
b = int(input(), 16)

result = no_carry_multiply(a, b)

print_hex256(result)
