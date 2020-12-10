/**
 * @file remotecall.hxx
 * @author savent (savent_gate@outlook.com)
 * @brief
 * @version 0.1
 * @date 2020-12-07
 *
 * Copyright 2020 jrlc
 *
 */
#pragma once

#include <loop.h>

#include <memory>
#include <utility>

// clang-format off
#include <platform/cJSON.hxx>
// clang-format on

namespace platform {

struct rcall_client {
 public:
  using val_t = mx_channel_t;

 public:
  rcall_client();
  explicit rcall_client(val_t* ch);
  explicit rcall_client(cJSON* obj);
  void from_json(cJSON* obj);
  cJSON* to_json();

  template <typename ReturnType>
  int call(int msg_id, const void* tx_data, size_t tx_len, ReturnType* rx_data, size_t* rx_len);
  int _call(int msg_id, const void* tx_data, size_t tx_len, void* rx_data, size_t* rx_len);

 private:
  val_t *ch_;
};

}  // namespace platform
