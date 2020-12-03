/**
 * @file entry.hxx
 * @author savent (savent_gate@outlook.com)
 * @brief entry of drivers
 * @version 0.1
 * @date 2020-11-23
 *
 * Copyright 2020 jrlc
 *
 */

#pragma once

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

#define func_entry(fn, level)                                         \
  static func_entry_t __func_entry_##level_##fn __attribute__((used)) \
      __attribute__((section(".initcall" #level ".init"))) = fn
#define driver_entry(fn, _name, _compat)                           \
  static driver_init_t __driver_entry_##fn __attribute__((used)) \
      __attribute__((section(".driverinit"))) = {.entry = (fn), .name = (_name), .compat = (_compat)}

#define func_entry_level_high(fn) func_entry(fn, 0)
#define func_entry_level_default(fn) func_entry(fn, 1)
#define func_entry_level_low(fn) func_entry(fn, 2)
#define func_entry_level_untest(fn) func_entry(fn, 3)

namespace platform {

struct entry {
  /**
   * @brief 初始化驱动框架的运行环境
   *
   * @return 0 初始化成功
   */
  static int platform_init();
};

}  // namespace platform
