#ifdef USE_WANGXIAO
#include "wangxiao.h"
#elif defined(USE_KARATSUBA)
#include "karatsuba.h"
#elif defined(USE_INTEL)
#include "intel.h"
#elif defined(USE_CLASSIC)
#include "classic.h"
#else
#include "gfmul.h" // 默认头文件
#endif