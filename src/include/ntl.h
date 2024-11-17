#define __ntl__ 
#include <NTL/GF2X.h>
#include <NTL/ZZ.h>
#include "common.h"
using namespace NTL;

inline void gfmul(__int128 A, __int128 B, __int128* res) {
    // Step 1: 将 __int128 转换为 GF2X 类型
    GF2X polyA, polyB;
    for (int i = 0; i < 128; ++i) {
        if (A & (__int128(1) << i)) SetCoeff(polyA, i);
        if (B & (__int128(1) << i)) SetCoeff(polyB, i);
    }

    // Step 2: 定义模多项式 x^128 + x^7 + x^2 + x + 1
    GF2X mod;
    SetCoeff(mod, 128);
    SetCoeff(mod, 7);
    SetCoeff(mod, 2);
    SetCoeff(mod, 1);
    SetCoeff(mod, 0);

    // Step 3: 计算乘积并模约简
    GF2X product;
    MulMod(product, polyA, polyB, mod); // 直接使用 NTL 的 MulMod

    // Step 4: 将结果转换回 __int128
    __int128 result = 0;
    for (int i = 0; i <= deg(product); ++i) {
        if (IsOne(coeff(product, i))) {
            result |= (__int128(1) << i);
        }
    }

    // Step 5: 输出结果
    *res = result;
}

#undef __ntl__
