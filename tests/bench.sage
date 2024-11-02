import time
from sage.all import PolynomialRing, GF

# 定义有限域上的多项式环
P.<x> = PolynomialRing(GF(2))

# 定义模多项式
mod_poly = x^32 + x^7 + x^3 + x^2 + 1

# 定义有限域 GF(2^128)
K.<a> = GF(2**32, modulus=mod_poly)

# 将整数转换为多项式
def int_to_poly(n):
    bin_str = bin(n)[2:]  # 将整数转换为二进制字符串，去掉'0b'前缀
    return sum([int(bit) * x^i for i, bit in enumerate(reversed(bin_str))])

# 将多项式转换回整数
def poly_to_int(p):
    return sum([int(coeff) * (2**i) for i, coeff in enumerate(p.list())])

# 从文件读取的数对
f_int = int(input(), 16)
g_int = int(input(), 16)

# 转换为多项式
f_poly = int_to_poly(f_int)
g_poly = int_to_poly(g_int)

# 在 GF(2^128) 上进行运算
f = K(f_poly)
g = K(g_poly)

# 乘法运算
mul_result = f * g

# 将结果转换回整数并显示为十六进制
mul_result_int = poly_to_int(mul_result.polynomial())
print(hex(mul_result_int)[2:].zfill(16))
