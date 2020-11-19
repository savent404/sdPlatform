/**
 * @file exception.hxx
 * @author savent (savent_gate@outlook.com)
 * @brief alter for std::functexcept
 * @version 0.1
 * @date 2020-11-19
 *
 * Copyright 2020 jrlc
 *
 */
#pragma once

#include <exception>

namespace platform::internal::alter {
__attribute__((noreturn)) static inline void throw_or_abort() {
  while (1) {
  }
}
}  // namespace platform::internal::alter
