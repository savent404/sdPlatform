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
