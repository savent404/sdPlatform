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

#include <string.h>

#include <cstddef>
#include <functional>
#include <tuple>
#include <variant>

// clang-format off
#include <platform/bits.hxx>
#include <platform/debug.hxx>
// clang-format on

namespace platform::syscall_dtl {

using bits = platform::bits;
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

template <typename T>
inline size_t size_of(T obj) {
  return sizeof(obj);
}
template <>
inline size_t size_of<char*>(char* ptr) {
  return strlen(ptr) + 1 + sizeof(size_t);
}
template <>
inline size_t size_of<const char*>(const char* ptr) {
  return strlen(ptr) + 1 + sizeof(size_t);
}
inline size_t size_of(void* ptr, size_t len) { return len + sizeof(len); }
template <typename... Args>
struct calculate_size {
  static size_t get(Args... args) { return 0; }
};
template <>
struct calculate_size<void*, size_t> {
  static size_t get(void* v1, size_t v2) { return size_of(v1, v2); }
};
template <typename... Args>
struct calculate_size<void*, size_t, Args...> {
  static size_t get(void* v1, size_t v2, Args... args) {
    return size_of(v1, v2) + calculate_size<Args...>::get(args...);
  }
};
template <typename T>
struct calculate_size<T> {
  static size_t get(T v1) { return size_of<T>(v1); }
};
template <typename T, typename... Args>
struct calculate_size<T, Args...> {
  static size_t get(T v1, Args... args) { return size_of<T>(v1) + calculate_size<Args...>::get(args...); }
};

template <typename T>
inline size_t do_pack(void* buf, T val) {
  auto sz = size_of<T>(val);
  memcpy(buf, &val, sz);
  return sz;
}
inline size_t do_pack(void* buf, void* ptr, size_t len) {
  memcpy(buf, &len, sizeof(len));
  buf = bits::shift_addr(buf, sizeof(len));
  memcpy(buf, ptr, len);
  return sizeof(len) + len;
}
template <>
inline size_t do_pack<const char*>(void* buf, const char* val) {
  size_t str_len = strlen(val) + 1;
  return do_pack(buf, reinterpret_cast<void*>(const_cast<char*>(val)), str_len);
}
template <>
inline size_t do_pack<char*>(void* buf, char* val) {
  size_t str_len = strlen(val) + 1;
  return do_pack(buf, reinterpret_cast<void*>(val), str_len);
}

template <typename... Args>
struct package_obj {
  static size_t set(void* buff, Args... args);
};
template <>
struct package_obj<void*, size_t> {
  static size_t set(void* buff, void* v1, size_t v2) { return do_pack(buff, v1, v2); }
};
template <typename T>
struct package_obj<T> {
  static size_t set(void* buff, T v) { return do_pack<T>(buff, v); }
};
template <typename... Args>
struct package_obj<void*, size_t, Args...> {
  static size_t set(void* buff, void* v1, size_t v2, Args... args) {
    auto shift = do_pack(buff, v1, v2);
    buff = bits::shift_addr(buff, shift);
    return shift + package_obj<Args...>::set(buff, args...);
  }
};
template <typename T, typename... Args>
struct package_obj<T, Args...> {
  static size_t set(void* buff, T v, Args... args) {
    auto shift = do_pack(buff, v);
    buff = bits::shift_addr(buff, shift);
    return shift + package_obj<Args...>::set(buff, args...);
  }
};

template <typename... Args>
char* package_param_to_buffer(size_t* buf_size, Args... args) {
  size_t sz = calculate_size<Args...>::get(args...);
  char* buf = new char[sz];
  size_t sz1 = package_obj<Args...>::set(buf, args...);
  debug::assert(sz == sz1);
  *buf_size = sz;
  return buf;
}

}  // namespace platform::syscall_dtl
