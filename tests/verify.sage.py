

# This file was *autogenerated* from the file verify.sage
from sage.all_cmdline import *   # import sage library

_sage_const_2 = Integer(2); _sage_const_128 = Integer(128); _sage_const_7 = Integer(7); _sage_const_1 = Integer(1); _sage_const_16 = Integer(16)# 定义有限域上的多项式环
P = PolynomialRing(GF(_sage_const_2 ), names=('x',)); (x,) = P._first_ngens(1)

# 定义模多项式
mod_poly = x**_sage_const_128  + x**_sage_const_7  + x**_sage_const_2  + x**_sage_const_1  + _sage_const_1 

# 定义有限域 GF(2^128)
K = GF(_sage_const_2 **_sage_const_128 , modulus=mod_poly, names=('a',)); (a,) = K._first_ngens(1)

# 将整数转换为多项式
def int_to_poly(n):
    bin_str = bin(n)[_sage_const_2 :]  # 将整数转换为二进制字符串，去掉'0b'前缀
    return sum([int(bit) * x**i for i, bit in enumerate(reversed(bin_str))])

# 将多项式转换回整数
def poly_to_int(p):
    return sum([int(coeff) * (_sage_const_2 **i) for i, coeff in enumerate(p.list())])

# 示例：整数表示的元素
f_int = int(input(), _sage_const_16 )
g_int = int(input(), _sage_const_16 )

# 转换为多项式
f_poly = int_to_poly(f_int)
g_poly = int_to_poly(g_int)

# 在 GF(2^128) 上进行运算
f = K(f_poly)
g = K(g_poly)

# 运算示例
add_result = f + g
mul_result = f * g

# 将结果转换回整数
add_result_int = poly_to_int(add_result.polynomial())
mul_result_int = poly_to_int(mul_result.polynomial())

print(hex(mul_result_int)[_sage_const_2 :])
