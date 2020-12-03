/**
 * @file xunsi-uart-demo.cxx
 * @author savent (savent_gate@outlook.com)
 * @brief
 * @version 0.1
 * @date 2020-11-30
 *
 * Copyright 2020 jrlc
 *
 */
#include <osal.h>
#include <platform.h>
#include <requirements.h>
#include <sunxi-uart.h>

#include <platform/device.hxx>
#include <platform/drivers/uart.hxx>
#include <platform/entry.hxx>

extern "C" void *sunxi_uart_echo_test(void *ptr) {
  auto id = reinterpret_cast<int>(ptr);

  const char msg[] = "sunxi-uart:Hello\r\n";
  dev_open(id, fflag::FF_READ | fflag::FF_WRITE);
  dev_write(id, msg, sizeof(msg));
  char ch[32];
  while (1) {
    os.os_msleep(10);
    auto cnt = dev_read(id, ch, 32);
    if (cnt > 0) {
      dev_write(id, ch, cnt);
    }
  }
  dev_close(id);
}

extern "C" int sunxi_uart_test_bind(void) {
  platform::device uart_1({{"name", "uart-1"},
                           {"compat", "arm,uart-sunxi,t3"},
                           {"config/base", (int)0x38000000 + 0x800},
                           {"config/range", 0x400},
                           {"config/irq", 34},
                           {"config/baudrate", 115200},
                           {"config/parity", "none"},
                           {"config/data_bits", 8},
                           {"config/stop_bits", 0},
                           {"config/uart_idx", 2}});
  auto str = uart_1.to_json_str();
  auto id = devmgr_create_device(str);
  platform::cJSON_free((void *)str);  //  NOLINT
  uart_1.set_id(id);

  str = uart_1.to_json_str();
  devmgr_update_device(id, str);
  platform::cJSON_free((void *)str);  //  NOLINT

  extern platform::drivers::uart::driver *sunxi_uart_driver;

  dev_bind(id, sunxi_uart_driver->get_id());

  // start a thread to handle uart echo
  os.os_thread_create(sunxi_uart_echo_test, reinterpret_cast<void*>(id));
  return 0;
}

driver_entry_level_untest(sunxi_uart_test_bind);
