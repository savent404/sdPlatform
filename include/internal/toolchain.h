#pragma once

#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif

// 暴露的C API
#define EXTERN_C_API

// 初始化程序段
#define __init_code __attribute__((constructor)) __attribute__((section("init-code")))

// 初始化数据段
#define __init_data __attribute__((section("init-data")))

// bytes align
#define __align(n) __attribute__((aligned(n)))
#define __aligned __align(sizeof(size_t))

// 寄存器
typedef void* reg_t;

// 拒绝编译器优化执行顺序
#define __IO(x) (x)

#define __weak __attribute__((weak))

#define BIT(x)  (1 << x)

#ifdef __cplusplus
}
#endif
