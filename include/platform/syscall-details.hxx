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
  /** Index: 3 */ std::function<int(int, const char*, size_t)>
  >;
// clang-format on

template <typename T, typename... Args>
using param_t = std::tuple<T, Args...>;

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
bool parse_param(T& t, void*& buf, size_t& len) {  // NOLINT
  if (len < sizeof(t)) return false;
  t = *reinterpret_cast<T*>(buf);
  len -= sizeof(t);
  // buf += sizeof(t);
  buf = static_cast<void*>(static_cast<char*>(buf) + sizeof(t));
  return true;
}

template <typename T>
bool do_parse(param_t<T>& ts, void*& buf, size_t& len) {  // NOLINT
  T val;
  if (!parse_param<T>(val, buf, len)) return false;
  ts = std::tie(val);
  return true;
}
template <typename T, typename... Args>
bool do_parse(param_t<T, Args...>& ts, void*& buf, size_t& len) {  // NOLINT
  T val;
  if (!parse_param<T>(val, buf, len)) return false;
  std::tuple<Args...> t;
  if (!do_parse(t, buf, len)) return false;
  ts = std::tuple_cat(std::tie(val), t);
  return true;
}

int call_fn_from_buffer(func_t func, void* buf, size_t len) {
  // as default, any error as syscall error
  int res = eno::ENO_SYSCALL_ERR;
  switch (func.index()) {
    // f -> std::funcion<int(void)>
    case 0: {
      auto f = std::get<0>(func);
      res = f();
    } break;

    // f -> std::function<int(int)>
    case 1: {
      auto f = std::get<1>(func);
      param_t<int> ts;
      do_parse<int>(ts, buf, len);
      res = f(std::get<0>(ts));
    } break;

    // f -> std::function<int(int, int)>
    case 2: {
      auto f = std::get<2>(func);
      param_t<int, int> ts;
      if (!do_parse<int, int>(ts, buf, len)) break;
      res = f(std::get<0>(ts), std::get<1>(ts));
    } break;

    default:
      res = eno::ENO_NOSYSCALL;
  }
  return res;
}

template <typename... Args>
int call_fn_from_param(func_t func, Args... args) {
  return eno::ENO_NOPERMIT;
}

}  // namespace platform::syscall_dtl
