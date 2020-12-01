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
  static inline constexpr T in(const T* addr) {
    return *reinterpret_cast<volatile const T*>(addr);
  }
  template <typename T1, typename T2 = T1>
  static inline constexpr void out(T1* addr, const T2 val) {
    *reinterpret_cast<volatile T1*>(addr) = static_cast<const T2>(val);
  }
  static inline constexpr int _shift_addr(const int addr, const int offset) { return addr + offset; }
  template <typename _Tp_ret = void*, typename _Tp_param = void*>
  static inline constexpr _Tp_ret shift_addr(_Tp_param addr, const int offset) {
    return reinterpret_cast<_Tp_ret>(_shift_addr(reinterpret_cast<int>(addr), offset));
  }
  template <typename T>
  static inline constexpr T shift_bits(const T val, const int offset) {
    return val << offset;
  }
  template <typename T = int>
  static inline constexpr T bit(const int offset) {
    return (1 << offset);
  }
  template <typename T = int>
  static inline constexpr T bitmask(const int len, const int offset = 0) {
    return ((1 << len) - 1) << offset;
  }
  template <typename T>
  static inline constexpr T set_bits(const T val, const int bit_offset, const int len = 1) {
    return val | bitmask<T>(len, bit_offset);
  }
  template <typename T>
  static inline constexpr T clear_bits(const T val, const int bit_offset, const int len = 1) {
    return val & ~bitmask<T>(len, bit_offset);
  }
  template <typename T>
  static inline constexpr int get_bits(const T val, const int bit_offset, const int len = 1) {
    return (val & bitmask<T>(len, bit_offset)) >> bit_offset;
  }
  template <typename T1, typename T2>
  static inline constexpr auto and_bits(const T1 v1, const T2 v2) {
    return v1 & v2;
  }
  template <typename T1, typename T2>
  static inline constexpr auto or_bits(const T1 v1, const T2 v2) {
    return v1 | v2;
  }
};
}  // namespace platform

#define BIT(n) (platform::bits::bit(n))
