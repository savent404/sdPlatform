/**
 * @file uart.hxx
 * @author savent (savent_gate@outlook.com)
 * @brief
 * @version 0.1
 * @date 2020-12-02
 *
 * Copyright 2020 jrlc
 *
 */
#pragma once

#include <osal.h>

#include <cstddef>
#include <cstdint>
#include <memory>
#include <utility>

// clang-format off
#include <consthash/crc32.hxx>
#include <platform/alter/queue.hxx>
#include <platform/device.hxx>
#include <platform/driver.hxx>
#include <platform/parameter.hxx>
#include <platform/runtime.hxx>
// clang-format on

/**
 * @brief Json demo
 * @code
 * {
 *  "name": "demo-uart",
 *  "compat": "arm,uart-demo",
 *  "config": {
 *      "base": 0x20000000,
 *      "range": 0x1000,
 *      "baudrate": 115200,
 *      "parity": "none", // 'none', 'odd', 'even'
 *      "data_bits": 8,
 *      "stop_bits": 0, // 0-1Bit, 1-1.5Bit, 2-2Bit
 *      "uart_idx": 0, // means uart-0
 *  }
 * }
 * @endcode
 */
namespace platform::drivers::uart {

using device_ref = platform::device::device_ref;
using driver_ref = platform::driver::driver_ref;
using initial_list = platform::parameters::initial_list;
// TODO(savent): enhance performance
using buffer_t = platform::alter::queue<char>;

struct runtime : public platform::runtime {
  RUNTIME_INIT(uart - runtime - structure);
  runtime() : pri_rt(nullptr) { __init(); }
  runtime(runtime&& other)
      : mem_base(other.mem_base),
        mem_range(other.mem_range),
        baudrate(other.baudrate),
        parity(other.parity),
        data_bits(other.data_bits),
        stop_bits(other.stop_bits),
        uart_idx(other.uart_idx),
        pri_rt(std::move(other.pri_rt)) { __init(); }

  void* mem_base;
  size_t mem_range;
  uint32_t baudrate;
  uint8_t parity;  // 0-none, 1-odd, 2-even
  uint8_t data_bits;
  uint8_t stop_bits;  // 0-1bit, 1-1.5bit, 2-2bit
  uint8_t uart_idx;   // 0~255

  int file_flag;
  buffer_t rx_buffer;
  buffer_t tx_buffer;
  platform::runtime::value_ptr pri_rt;
};

using runtime_ref = runtime&;
using runtime_ptr = runtime*;

struct api {
  int (*match)(runtime_ptr);     // 检查硬件是否匹配
  int (*setup)(runtime_ptr);     // 初始化硬件
  int (*shutdown)(runtime_ptr);  // 停用硬件

  int (*start_tx)(runtime_ptr);
  int (*stop_tx)(runtime_ptr);
  int (*poll_tx)(runtime_ptr);

  int (*start_rx)(runtime_ptr);
  int (*stop_rx)(runtime_ptr);  // 停止接收流程
  int (*poll_rx)(runtime_ptr, char*, size_t);

  int (*pm)(runtime_ptr, int state);  // 暂停/回复

  int (*config_parity)(runtime_ptr ptr);
  int (*config_data_bit)(runtime_ptr ptr);
  int (*config_stop_bit)(runtime_ptr ptr);
  int (*config_baud_rate)(runtime_ptr ptr);

  int (*ioctl)(runtime_ptr, int cmds, void* in_out, size_t* in_out_len,
               size_t buffer_max);  // 硬件提供的额外的ioctl功能
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

 protected:
  bool load_device_runtime(parameters_ref device_conf, runtime* rt);

 private:
  api api_;
};

}  // namespace platform::drivers::uart
