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
#include <sunxi-uart.h>
#include <platform/drivers/uart.hxx>
#include <platform/entry.hxx>
#include <platform.h>
#include <requirements.h>
#include <platform/device.hxx>

extern "C" int sunxi_uart_test_bind(void) {
  platform::device uart_1({{"name", "uart-1"},
                           {"compat", "arm,uart-sunxi,t3"},
                           {"config/base", (int)0x38000000 + 20},
                           {"config/range", 0x400},
                           {"config/irq", 34},
                           {"config/baudrate", 115200},
                           {"config/parity", 0},
                           {"config/data_bits", 8}});
  auto str = uart_1.to_json_str();
  auto id = devmgr_create_device(str);
  platform::cJSON_free((void*)str);  //  NOLINT
  uart_1.set_id(id);

  str = uart_1.to_json_str();
  devmgr_update_device(id, str);
  platform::cJSON_free((void*)str);  //  NOLINT

  extern platform::drivers::uart::driver *sunxi_uart_driver;

  dev_bind(id, sunxi_uart_driver->get_id());

  const char msg[] = "Hello\r\n";
  while (1)
    dev_write(id, msg, sizeof(msg));

  return 0;
}

driver_entry_level_untest(sunxi_uart_test_bind);
