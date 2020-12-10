/**
 * @file msg.hxx
 * @author your name (you@domain.com)
 * @brief 用于封装/拆封消息
 * @version 0.1
 * @date 2020-12-09
 *
 * @copyright Copyright (c) 2020
 *
 */
#pragma once

#include <smempool.h>
#include <string.h>

#include <cstddef>
#include <tuple>
#include <utility>

// clang-format off
#include <platform/debug.hxx>
#include <platform/syscall-details.hxx>
// clang-format on

namespace platform {

struct msg_wrapper;
struct msg {
 public:
  msg() : buffer_(nullptr), sz_(0) {}
  explicit msg(size_t sz) : buffer_(new char[sz]), sz_(sz) {}
  msg(const msg &other) : buffer_(new char[other.sz_]), sz_(other.sz_) { memcpy(buffer_, other.buffer_, sz_); }
  msg(msg &&other) : buffer_(std::move(other.buffer_)), sz_(other.sz_) {
    other.buffer_ = nullptr;
    other.sz_ = 0;
  }
  msg& operator=(msg&& other) {
    if (buffer_) {
      delete[] buffer_;
      buffer_ = nullptr;
      sz_ = 0;
    }
    buffer_  = other.buffer_;
    sz_ = other.sz_;
    other.buffer_ = nullptr;
    other.sz_ = 0;
    return *this;
  }
  ~msg() {
    if (buffer_) {
      delete[] buffer_;
    }
  }
  template <typename T = void *>
  T get() {
    return static_cast<T>(buffer_);
  }
  size_t size() { return sz_; }

 private:
  friend struct msg_wrapper;
  char *buffer_;
  size_t sz_;
};

struct msg_wrapper {
 public:
  template <typename... Args>
  static msg package(Args... args) {
    size_t sz = syscall_dtl::calculate_size<Args...>::get(args...);
    msg buff(sz);
    size_t sz1 = syscall_dtl::package_obj<Args...>::set(buff.get(), args...);
    debug::assert(sz == sz1);
    return std::move(buff);
  }
  template <typename... Args>
  static auto &unpackage(const msg &buff) {
    std::tuple<Args...> ts;
    void *ptr = buff.buffer_;
    size_t sz = buff.sz_;
    debug::assert(syscall_dtl::do_parse<Args...>(ts, ptr, sz));
    return ts;
  }

 private:
};

}  // namespace platform
