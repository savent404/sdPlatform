/**
 * @file msg_wrapper.hxx
 * @author savent (savent_gate@outlook.com)
 * @brief 提供msg的封装/查封方法
 * @version 0.1
 * @date 2020-12-14
 *
 * Copyright 2020 jrlc
 *
 */
#pragma once
// clang-format off
#include <tuple>
#include <utility>
// clang-format on

#include <platform/debug.hxx>
#include <platform/msg.hxx>
#include <platform/syscall-details.hxx>

namespace platform {
/**
 * @brief 提供msg的封装/拆封方法
 *
 */
struct msg_wrapper {
 public:
  /**
   * @brief 打包参数为msg
   *
   * @tparam Args 参数类型
   * @param args 简单类型参数
   * @return msg
   * @note 模板类型可根据各个参数类型自行推导，可不填写模板类型
   * @code
   * int a = 0;
   * const char * b = "hello";
   * auto package = msg_wrapper::package(a, b);
   * @endcode
   */
  template <typename... Args>
  static msg package(Args... args) {
    size_t sz = syscall_dtl::calculate_size<Args...>::get(args...);
    msg buff(sz);
    size_t sz1 = syscall_dtl::package_obj<Args...>::set(buff.get(), args...);
    debug::assert(sz == sz1);
    // return std::move(buff);
    // seems like cpp treat it as a rvalue
    return buff;
  }
  /**
   * @brief 拆包msg 为tuple类型
   *
   * @tparam Args 参数类型（按照打包时的顺序申明)
   * @param buff
   * @return std::tuple<Args...>
   * @code
   * auto res = msg_wrapper::unpackage<int, const char*>(package);
   * int a = std::get<0>(res);
   * const char * b = std::get<1>(res);
   * @endcode
   */
  template <typename... Args>
  static auto unpackage(const msg &buff) {
    std::tuple<Args...> ts;
    void *ptr = buff.buffer_;
    size_t sz = buff.sz_;
    debug::assert(syscall_dtl::do_parse<Args...>(ts, ptr, sz));
    return std::move(ts);
  }

 private:
};

}  // namespace platform
