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
#include <memory>
#include <utility>

// clang-format off
#include <platform/debug.hxx>
// clang-format on

namespace platform {

struct msg_wrapper;
struct msg {
 public:
  msg() : buffer_(nullptr), sz_(0) {}
  msg(void* base_addr, size_t sz) : buffer_(static_cast<char*>(base_addr)), sz_(sz) {}
  explicit msg(size_t sz) : buffer_(new char[sz]), sz_(sz) {}
  msg(const msg &other) : buffer_(new char[other.sz_]), sz_(other.sz_) { memcpy(buffer_, other.buffer_, sz_); }
  msg(msg &&other) : buffer_(std::move(other.buffer_)), sz_(other.sz_) {
    other.buffer_ = nullptr;
    other.sz_ = 0;
  }
  msg &operator=(msg &&other) {
    if (buffer_) {
      delete[] buffer_;
      buffer_ = nullptr;
      sz_ = 0;
    }
    buffer_ = other.buffer_;
    sz_ = other.sz_;
    other.buffer_ = nullptr;
    other.sz_ = 0;
    return *this;
  }
  virtual ~msg() {
    if (buffer_) {
      delete[] buffer_;
    }
  }
  template <typename T = void *>
  T get() {
    return static_cast<T>(buffer_);
  }
  template <typename T = void *>
  const T get() const {
    return static_cast<T>(buffer_);
  }
  size_t size() const { return sz_; }
  bool resize(size_t new_sz, bool realloc = true) {
    size_t min_sz = new_sz > sz_ ? sz_ : new_sz;
    if (realloc) {
      auto p = new char[new_sz];
      if (!p) return false;
      memcpy(p, buffer_, min_sz);
      delete[] buffer_;
      buffer_ = p;
    }
    sz_ = new_sz;
    return true;
  }

 protected:
  friend struct msg_wrapper;
  char *buffer_;
  size_t sz_;
};

struct msg_ref : public msg {
  msg_ref(void* base, size_t sz) : msg(0) {
    buffer_ = static_cast<char*>(base);
    sz_ = sz;
  }
  virtual ~msg_ref() {
    buffer_ = nullptr;
    sz_ = 0;
  }
};

using msg_ptr = std::unique_ptr<msg>;
using mst_ref_ptr = std::unique_ptr<msg_ref>;

}  // namespace platform
