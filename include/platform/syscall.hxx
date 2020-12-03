/**
 * @file syscall.hxx
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

// clang-format off
#include <consthash/crc32.hxx>
#include <platform/alter/map.hxx>
#include <platform/alter/string.hxx>
#include <platform/cJSON.hxx>
#include <platform/syscall-details.hxx>
// clang-format on

namespace platform {

struct syscall {
 public:
  using func_t = syscall_dtl::func_t;
  using hash_id = size_t;
  using string = alter::string;  // NOLINT

 public:
  syscall() = default;
  ~syscall() = delete;

  bool add(string func_name, func_t func);
  bool del(string func_name);

  static hash_id hash(const string& func_name) { return consthash::crc32(func_name.data(), func_name.length()); }
  constexpr static hash_id hash(const char* func_name) { return consthash::crc32(func_name, _strlen(func_name)); }

  int call(hash_id id, void* buf, size_t len) const;

  template <typename... Args>
  int call_p(hash_id id, Args&... args) const;

  static syscall* get_instance();

  cJSON* get_ipc_description();
  void set_ipc_description(const void*);

 private:
  func_t find(hash_id func_id) const;
  bool add(hash_id func_id, func_t func);
  bool del(hash_id func_id);
  constexpr static int _strlen(const char* name) { return *name ? _strlen(name + 1) + 1 : 0; }

  using kv_t = alter::map<hash_id, func_t>;
  kv_t kv_;
};

}  // namespace platform
