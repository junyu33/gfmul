from sage.all import *

# 定义函数来生成项数特定的不可约多项式
def find_min_term_irreducible_polynomial(n: int):
    R = PolynomialRing(GF(2), 'x')
    x = R.gen()

    # 尝试项数为3的多项式：形式如 x^n + x^k + 1
    for k in range(1, n):
        poly = x^n + x^k + 1
        if poly.is_irreducible():
            return poly

    # 尝试项数为5的多项式：形式如 x^n + x^k1 + x^k2 + x^k3 + 1
    for k1 in range(1, n):
        for k2 in range(1, k1):
            for k3 in range(1, k2):
                poly = x^n + x^k1 + x^k2 + x^k3 + 1
                if poly.is_irreducible():
                    return poly

    return None

n = int(input('input the level of your field (8 <= n <= 128): '))
if n < 8 or n > 128:
    print('Invalid input')
    exit(1)

irreducible_polynomials = {}
poly = find_min_term_irreducible_polynomial(n)
if poly:
    degrees = sorted(poly.exponents(), reverse=True)
    for degree in degrees:
        print(degree, end = ' ')



'''
print('[', end='')
for degree in degrees:
    if degree == 0:
        print(0, end='')
        break
    print(degree, end=', ')
print(']')  # 空行分隔不同的域
'''
