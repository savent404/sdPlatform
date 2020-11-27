/**
 * @file bits.hxx
 * @author savent (savent_gate@outlook.com)
 * @brief
 * @version 0.1
 * @date 2020-11-26
 *
 * Copyright 2020 jrlc
 *
 */
#pragma once
#include <cstdint>

namespace platform {

struct bits {
  template <typename T>
  static constexpr inline T in(void* addr) {
    return reinterpret_cast<T*>(addr);
  }
  template <typename T>
  static constexpr inline void out(T* addr, const T val) {
    *reinterpret_cast<T*>(addr) = val;
  }

  template <typename T>
  static constexpr inline T* io_addr_shift(void* addr, int offset) {
    return reinterpret_cast<T*>(reinterpret_cast<char*>(addr) + offset);
  }

  template <typename T = int>
  static constexpr T bit(int offset) {
    return (1 << offset);
  }

  template <typename T>
  static constexpr T set_bit(T val, int bit_offset) {
    return val | bit(bit_offset);
  }

  template <typename T>
  static constexpr T clear_bit(T val, int bit_offset) {
    return val & (~bit(bit_offset));
  }
};
}  // namespace platform

#define BIT(n) (platform::bits::bit(n))
