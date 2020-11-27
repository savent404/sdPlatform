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
typedef int (*_driver_entry_t)(void);
}

using driver_entry_t = _driver_entry_t;

#define driver_entry(fn, level)                                           \
  static driver_entry_t __driver_entry_##level_##fn __attribute__((used)) \
      __attribute__((section(".initcall" #level ".init"))) = fn

#define driver_entry_level_high(fn) driver_entry(fn, 0)
#define driver_entry_level_default(fn) driver_entry(fn, 1)
#define driver_entry_level_low(fn) driver_entry(fn, 2)
#define driver_entry_level_untest(fn) driver_entry(fn, 3)

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
