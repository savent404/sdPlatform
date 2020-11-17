/**
 * @file runtime.hxx
 * @author savent (savent_gate@outlook.com)
 * @brief
 * @version 0.1
 * @date 2020-11-17
 *
 * Copyright 2020 jrlc
 *
 */
#pragma once

#include <memory>

namespace platform {

struct runtime {
 public:
  using value_ref = runtime &;
  using value_ptr = std::unique_ptr<runtime>;
  void *tp;
  runtime() = default;
  ~runtime() = default;

  /**
   * @brief convert this instance to cJSON obj
   * @return cJSON*
   */
  virtual cJSON *toJson() const { return nullptr; }
};

}  // namespace platform
