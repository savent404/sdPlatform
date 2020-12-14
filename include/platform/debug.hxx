/**
 * @file debug.hxx
 * @author savent (savent_gate@outlook.com)
 * @brief
 * @version 0.1
 * @date 2020-12-01
 *
 * Copyright 2020 jrlc
 *
 */
#pragma once

namespace platform {

struct debug {
  /**
   * @brief 内部断言
   * 
   * @param val 
   */
  static void assert(bool val) {
    while (!val) {
    }
  }
};

}  // namespace platform
