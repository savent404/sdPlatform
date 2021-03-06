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
#include <memory>
#include <tuple>
#include <variant>

// clang-format off
#include <platform/bits.hxx>
#include <platform/debug.hxx>
#include <platform/msg.hxx>
// clang-format on

namespace platform::syscall_dtl {

using bits = platform::bits;
using msg = platform::msg;
// clang-format off
using func_t = std::variant<
  /** Index: 0 */ std::function<int(void)>,
  /** Index: 1 */ std::function<int(int)>,
  /** Index: 2 */ std::function<int(int, int)>,
  /** Index: 3 */ std::function<int(int, const char*, size_t)>,
  /** Index: 4 */ std::function<int(int, void*, size_t)>,
  /** Index: 5 */ std::function<int(int, void*, size_t, void*, size_t)>,
  /** Index: 6 */ std::function<int(int, int, void*, size_t, size_t *)>,
  /** Index: 7 */ std::function<int(int, void*, size_t, msg*)>,
  /** Index: 8 */ std::function<int(int, msg*)>,
  /** Index: 9 */ std::function<int(int, int, void*, size_t, msg*)>
  >;
// clang-format on

template <typename... Args>
int call_fn_from_param(func_t func, Args... args) {
  return -1;
}
int call_fn_from_buffer(func_t func, void* buf, size_t len, msg* out);

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
template <>
inline size_t size_of<const msg*>(const msg* ptr) {
  return sizeof(size_t);
}
template <>
inline size_t size_of<msg*>(msg* ptr) {
  return size_of<const msg*>(ptr);
}
inline size_t size_of(const void* ptr, size_t len) { return len + sizeof(len); }
template <typename... Args>
struct calculate_size {
  static size_t get(Args... args) { return 0; }
};
template <>
struct calculate_size<void*, size_t> {
  static size_t get(void* v1, size_t v2) { return size_of(v1, v2); }
};
template <>
struct calculate_size<const void*, size_t> {
  static size_t get(const void* v1, size_t v2) { return size_of(v1, v2); }
};
template <typename... Args>
struct calculate_size<void*, size_t, Args...> {
  static size_t get(void* v1, size_t v2, Args... args) {
    return size_of(v1, v2) + calculate_size<Args...>::get(args...);
  }
};
template <typename... Args>
struct calculate_size<const void*, size_t, Args...> {
  static size_t get(const void* v1, size_t v2, Args... args) {
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
inline size_t do_pack(void* buf, const void* ptr, size_t len) {
  memcpy(buf, &len, sizeof(len));
  buf = bits::shift_addr(buf, sizeof(len));
  memcpy(buf, ptr, len);
  return sizeof(len) + len;
}
template <>
inline size_t do_pack<const msg*>(void* buf, const msg* val) {
  return do_pack<size_t>(buf, val->size());
}
template <>
inline size_t do_pack<msg*>(void* buf, msg* val) {
  return do_pack<const msg*>(buf, val);
}
template <>
inline size_t do_pack<const msg_ref*>(void* buf, const msg_ref* val) {
  return do_pack<const msg*>(buf, val);
}
template <>
inline size_t do_pack<msg_ref*>(void* buf, msg_ref* val) {
  return do_pack<const msg*>(buf, val);
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
template <>
struct package_obj<const void*, size_t> {
  static size_t set(void* buff, const void* v1, size_t v2) { return do_pack(buff, v1, v2); }
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
template <typename... Args>
struct package_obj<const void*, size_t, Args...> {
  static size_t set(void* buff, const void* v1, size_t v2, Args... args) {
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

template <typename T, typename... Args>
using param_t = std::tuple<T, Args...>;

/**
 * @brief 将指针后移
 *
 * @param pointer 指针
 * @param offset  位移大小(字节为单位)
 */
// static void __attribute__((used)) shift_buffer_point(const void*& pointer, size_t offset) {  // NOLINT
//   pointer = bits::shift_addr<const void*>(pointer, offset);
// }
inline void shift_buffer_point(void*& pointer, size_t offset) {  // NOLINT
  pointer = bits::shift_addr<void*>(pointer, offset);
}

/**
 * @brief 从buffer中获取一个数据
 *
 * @tparam T 数据类型
 * @param[out] t 输出的数据
 * @param[in/out] buf
 * @param[in/out] len
 * @note 该函数仅能解析基础数据类型，复杂类型需要特例化该函数
 * @note 该函数会位移buff指针指向下一个数据起始位置
 * @note 该函数会减去*len的大小，指示buf剩余字节长度
 * @return true 获取成功
 * @return false 获取失败
 */
template <typename T>
inline bool parse_param(T& t, void*& buf, size_t& len) {  // NOLINT
  if (len < sizeof(t)) return false;
  t = *reinterpret_cast<T*>(buf);
  len -= sizeof(t);
  shift_buffer_point(buf, sizeof(t));
  return true;
}
inline bool parse_param(void*& t1, size_t& t2, void*& buf, size_t& len) {  // NOLINT
  size_t buf_len = 0;
  parse_param(buf_len, buf, len);
  if (len < buf_len) return false;

  t2 = buf_len;
  t1 = reinterpret_cast<void*>(buf);
  len -= buf_len;
  shift_buffer_point(buf, buf_len);
  return true;
}
inline bool parse_param(const void*& t1, size_t& t2, void*& buf, size_t& len) {  // NOLINT
  size_t buf_len = 0;
  parse_param(buf_len, buf, len);
  if (len < buf_len) return false;

  t2 = buf_len;
  t1 = reinterpret_cast<const void*>(buf);
  len -= buf_len;
  shift_buffer_point(buf, buf_len);
  return true;
}
/**
 * @brief convert 'const char*'
 *
 * @tparam
 * @param t
 * @param buf
 * @param len
 * @return true
 * @return false
 */
template <>
inline bool parse_param(const char*& t, void*& buf, size_t& len) {  // NOLINT
  // capture size from head
  size_t str_len = 0;
  parse_param(str_len, buf, len);

  if (len < str_len) return false;
  t = reinterpret_cast<const char*>(buf);
  len -= str_len;
  shift_buffer_point(buf, str_len);
  return true;
}

template <>
inline bool parse_param(void*& t, void*& buf, size_t& len) {  // NOLINT
  // capture size from head
  size_t size_len = 0;
  parse_param(size_len, buf, len);

  if (len < size_len) return false;
  t = reinterpret_cast<void*>(buf);
  len -= size_len;
  shift_buffer_point(buf, size_len);
  return true;
}

template <>
inline bool parse_param(msg*& t, void*& buf, size_t& len) {  // NOLINT
  // capture size from head
  size_t sz;
  auto res = parse_param<size_t>(sz, buf, len);
  if (!res) return false;
  t = std::move(new msg(sz));
  return true;
}
template <>
inline bool parse_param(msg_ptr& t, void*& buf, size_t& len) {  // NOLINT
  // cpature size from headk
  msg* tp = nullptr;
  auto res = parse_param<msg*>(tp, buf, len);
  if (!res) return false;
  t = msg_ptr(tp);
  return true;
}

template <typename T, typename... Args>
struct parse_obj {
  static bool parse(param_t<T, Args...>& ts, void*& buff, size_t& len) {  // NOLINT
    T val;
    if (!parse_param<T>(val, buff, len)) return false;
    std::tuple<Args...> t;
    if (!parse_obj<Args...>::parse(t, buff, len)) return false;
    ts = std::tuple_cat(std::tie(val), t);
    return true;
  }
};
template <>
struct parse_obj<void*, size_t> {
  static bool parse(param_t<void*, size_t>& ts, void*& buf, size_t& len) {  // NOLINT
    void* v1;
    size_t v2;
    if (!parse_param(v1, v2, buf, len)) return false;
    std::tuple<void*, size_t> t;
    ts = std::tuple_cat(std::tie(v1), std::tie(v2));
    return true;
  }
};
template <>
struct parse_obj<const void*, size_t> {
  static bool parse(param_t<const void*, size_t>& ts, void*& buf, size_t& len) {  // NOLINT
    const void* v1;
    size_t v2;
    if (!parse_param(v1, v2, buf, len)) return false;
    std::tuple<void*, size_t> t;
    ts = std::tuple_cat(std::tie(v1), std::tie(v2));
    return true;
  }
};
template <typename... Args>
struct parse_obj<void*, size_t, Args...> {
  static bool parse(param_t<void*, size_t, Args...>& ts, void*& buf, size_t& len) {  // NOLINT
    std::tuple<void*, size_t> t1;
    std::tuple<Args...> t2;
    if (!parse_obj<void*, size_t>::parse(t1, buf, len)) return false;
    if (!parse_obj<Args...>::parse(t2, buf, len)) return false;
    ts = std::tuple_cat(t1, t2);
    return true;
  }
};
template <typename... Args>
struct parse_obj<const void*, size_t, Args...> {
  static bool parse(param_t<const void*, size_t, Args...>& ts, void*& buf, size_t& len) {  // NOLINT
    std::tuple<const void*, size_t> t1;
    std::tuple<Args...> t2;
    if (!parse_obj<const void*, size_t>::parse(t1, buf, len)) return false;
    if (!parse_obj<Args...>::parse(t2, buf, len)) return false;
    ts = std::tuple_cat(t1, t2);
    return true;
  }
};
template <typename T>
struct parse_obj<T> {
  static bool parse(param_t<T>& ts, void*& buff, size_t& len) {  // NOLINT
    T val;
    if (!parse_param(val, buff, len)) return false;
    ts = std::tie(val);
    return true;
  }
};
// template <typename T, typename... Args>
// struct parse_obj<T, Args...> {
//   static bool parse(param_t<T, Args...>& ts, void*& buff, size_t& len) {  // NOLINT
//     T val;
//     if (!parse_param<T>(val, buff, len)) return false;
//     std::tuple<Args...> t;
//     if (!parse_obj<Args...>::parse(t, buf, len)) return false;
//     ts = std::tuple_cat(std::tie(val), t);
//     return true;
//   }
// };
// template <typename T>
// inline bool do_parse(param_t<T>& ts, void*& buf, size_t& len) {  // NOLINT
//   T val;
//   if (!parse_param<T>(val, buf, len)) return false;
//   ts = std::tie(val);
//   return true;
// }
// template <typename T, typename... Args>
// inline bool do_parse(param_t<T, Args...>& ts, void*& buf, size_t& len) {  // NOLINT
//   T val;
//   if (!parse_param<T>(val, buf, len)) return false;
//   std::tuple<Args...> t;
//   if (!do_parse(t, buf, len)) return false;
//   ts = std::tuple_cat(std::tie(val), t);
//   return true;
// }
template <typename T, typename... Args>
inline bool do_parse(param_t<T, Args...>& ts, void*& buf, size_t& len) {  // NOLINT
  return parse_obj<T, Args...>::parse(ts, buf, len);
}

}  // namespace platform::syscall_dtl
