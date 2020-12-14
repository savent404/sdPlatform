/**
 * @file entry.hxx
 * @author savent (savent_gate@outlook.com)
 * @brief 提供驱动进程的启动方法
 * @version 0.1
 * @date 2020-11-23
 *
 * Copyright 2020 jrlc
 *
 */

#pragma once

#include <loop.h>
#include <platform/cJSON.hxx>

extern "C" {
typedef int (*_func_entry_t)(void);
typedef int (*_driver_entry_t)(void*);
typedef struct {
  _driver_entry_t entry;
  const char* name;
  const char* compat;
} _driver_init_t;
}

using func_entry_t = _func_entry_t;
using driver_entry_t = _driver_entry_t;
using driver_init_t = _driver_init_t;

/**
 * @brief 自启动函数入口
 * 
 * @param fn 函数指针 func_entry_t
 * @param level 启动优先级， 0为最高优先级， 3为最低
 * 
 */
#define func_entry(fn, level)                                         \
  static func_entry_t __func_entry_##level_##fn __attribute__((used)) \
      __attribute__((section(".initcall" #level ".init"))) = fn
/**
 * @brief 驱动进程初始化入口
 * 
 * @param fn 驱动进程初始化函数 driver_entry_t
 * @param _name 驱动名称
 * @param _compat compat字段
 */
#define driver_entry(fn, _name, _compat)                         \
  static driver_init_t __driver_entry_##fn __attribute__((used)) \
      __attribute__((section(".driverinit"))) = {.entry = (fn), .name = (_name), .compat = (_compat)}

// 自启动函数入口
#define func_entry_level_high(fn) func_entry(fn, 0)
#define func_entry_level_default(fn) func_entry(fn, 1)
#define func_entry_level_low(fn) func_entry(fn, 2)
#define func_entry_level_untest(fn) func_entry(fn, 3)

namespace platform {

struct entry {
 public:
  /**
   * @brief 初始化驱动框架的运行环境
   *
   * @param ipc_ch a channel chat with device manager
   *
   * @return 0 初始化成功
   */
  static int platform_init(void* ipc_ch);
};

}  // namespace platform
