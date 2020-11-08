#pragma once

// C/C++的C内容声明开头
#define __cbegin

// C/C++的C内容声明结尾
#define __cend

// 暴露的C API
#define EXTERN_C_API

// 初始化程序段
#define __init_code

// 初始化数据段
#define __init_data

// 寄存器
typedef void* reg_t;

// CPU 32/64位
typedef unsigned int cpu_t;

// 拒绝编译器优化执行顺序
#define __IO(x) (x)
