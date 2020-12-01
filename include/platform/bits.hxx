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
  static constexpr inline T in(const T* addr) {
    return *reinterpret_cast<volatile const T*>(addr);
  }
  template <typename T1, typename T2=T1>
  static constexpr inline void out(T1* addr, const T2 val) {
    *reinterpret_cast<volatile T1*>(addr) = static_cast<const T2>(val);
  }
  template <typename T>
  static constexpr inline T* io_addr_shift(void* addr, const int offset) {
    return reinterpret_cast<T*>(reinterpret_cast<char*>(addr) + offset);
  }

  template <typename T = int>
  static constexpr T bit(const int offset) {
    return (1 << offset);
  }

  template <typename T>
  static constexpr T set_bit(const T val, const int bit_offset) {
    return val | bit(bit_offset);
  }

  template <typename T>
  static constexpr T clear_bit(const T val, const int bit_offset) {
    return val & (~bit(bit_offset));
  }
  template <typename T>
  static constexpr int get_bit(const T val, const int bit_offset) {
    return (val & bit<T>(bit_offset)) ? 1 : 0;
  }
  template <typename T>
  static constexpr T and_bit(T v1, T v2) {
    return v1 & v2;
  }
  template <typename T>
  static constexpr T or_bit(const T v1, const T v2) {
    return v1 | v2;
  }

};
}  // namespace platform

#define BIT(n) (platform::bits::bit(n))
