/**
 * @file rcc.hxx
 * @author savent (savent_gate@outlook.com)
 * @brief
 * @version 0.1
 * @date 2020-12-03
 *
 * Copyright 2020 jrlc
 *
 */
#pragma once

#include <platform/device.hxx>
#include <platform/driver.hxx>
#include <platform/runtime.hxx>

namespace platform::drivers::rcc {

struct runtime : public platform::runtime {
  RUNTIME_INIT(rcc_runtime_structure);
  runtime() { __init(); }

  void* mem_base;
  size_t mem_range;

  int file_flag;
};

using runtime_ref = runtime&;
using runtime_ptr = runtime*;

struct api {
  int (*match)(runtime_ptr);
  int (*setup)(runtime_ptr);
  int (*shutdown)(runtime_ptr);

  int (*config_gate_enable)(runtime_ptr, int idx, bool);
};

struct driver : public platform::driver {
 public:
  driver(initial_list kv_list, const api& api);
  ~driver();

 protected:
  int init_(int argc, char** argv) override;
  int deinit_() override;
  int bind_(device_ref dev) override;
  int unbind_(device_ref dev) override;
  int open_(device_ref dev, int flags) override;
  int close_(device_ref dev) override;
  int transfer_(device_ref dev, const void* in, size_t in_len, void* out, size_t out_len) override;
  int write_(device_ref dev, const void* out, size_t len) override;
  int read_(device_ref dev, void* out, size_t len) override;
  int ioctl_(device_ref, int cmds, void* in_out, size_t* in_out_len, size_t buffer_max) override;
  platform::cJSON* to_json_() override;
  void from_json_(platform::cJSON*) override;
  void register_internal_syscall_() override;
};

}  // namespace platform::drivers::rcc
