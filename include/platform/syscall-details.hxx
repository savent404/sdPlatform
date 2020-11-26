/**
 * @file syscall-details.hxx
 * @author savent (savent_gate@outlook.com)
 * @brief
 * @version 0.1
 * @date 2020-11-20
 *
 * Copyright 2020 jrlc
 *
 */

#pragma once

#include <cstddef>
#include <functional>
#include <tuple>
#include <variant>

namespace platform::syscall_dtl {

// clang-format off
using func_t = std::variant<
  /** Index: 0 */ std::function<int(void)>,
  /** Index: 1 */ std::function<int(int)>,
  /** Index: 2 */ std::function<int(int, int)>,
  /** Index: 3 */ std::function<int(int, const char*, size_t)>,
  /** Index: 4 */ std::function<int(int, void*, size_t)>,                 // dev_write & dev_read
  /** Index: 5 */ std::function<int(int, void*, size_t, void*, size_t)>,  // dev_transfer
  /** Index: 6 */ std::function<int(int, int, void*, size_t *, size_t)>   // dev_ioctl
  >;
// clang-format on

template <typename... Args>
int call_fn_from_param(func_t func, Args... args) {
  return -1;
}
int call_fn_from_buffer(func_t func, void* buf, size_t len);

}  // namespace platform::syscall_dtl
