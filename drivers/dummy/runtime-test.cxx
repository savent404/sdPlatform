/**
 * @file runtime-test.cxx
 * @author savent (savent_gate@outlook.com)
 * @brief
 * @version 0.1
 * @date 2020-11-26
 *
 * Copyright 2020 jrlc
 *
 */
#include <initcall.h>
#include <platform/runtime.hxx>
#include <consthash/crc32.hxx>

/**
 * @brief 创建新的runtime结体
 * 
 * @note 在struct开头使用魔法 "RUNTIME_INIT(<自定义名字>)", 需要确保自定义名字全局唯一
 * @note 为新的runtime创建一个新的默认构造函数， 调用 __init() 函数
 */
struct myruntime : public platform::runtime {
  RUNTIME_INIT(test-runtime);
  myruntime() { __init(); }
  int a;
  int b;
  int c;
};


extern "C" int test_case_runtime() {
    platform::runtime rt1;
    myruntime rt2;

    myruntime rt3(rt2);

    auto rt = rt2.promote<myruntime>();
    return 0;
}

lvl0_initcall(test_case_runtime);

