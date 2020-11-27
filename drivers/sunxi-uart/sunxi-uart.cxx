#include <sunxi-uart.h>
#include <platform/drivers/uart.hxx>
#include <platform/entry.hxx>

// headers only for test
#include <platform.h>
#include <requirements.h>
#include <platform/bits.hxx>
#include <platform/device.hxx>

using namespace platform::drivers::uart;  // NOLINT

static int sunxi_uart_setup(runtime_ptr rt) { return eno::ENO_OK; }

static int sunxi_uart_tx(runtime_ptr rt, const char* out, size_t len) {
  char* base = platform::bits::io_addr_shift<char>(rt->mem_base, SUNXI_UART_THR);
  while (len--) platform::bits::out(base, *out--);
  return eno::ENO_OK;
}

struct api sunxi_uart_api = {
    .match = nullptr,
    .setup = sunxi_uart_setup,
    .shutdown = nullptr,
    .tx = sunxi_uart_tx,
};

static int driver_id = 0;

extern "C" int sunxi_uart_test_bind(void) {
  platform::device uart_1({{"name", "uart-1"},
                           {"compat", "arm,uart-sunxi,t3"},
                           {"config/base", (int)0x38000000 + 0x800},
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

  dev_bind(id, driver_id);

  const char msg[] = "Hello\r\n";
  dev_write(id, msg, sizeof(msg));

  return 0;
}

extern "C" int sunxi_uart_entry() {
  platform::entry::platform_init();
  driver sunxi_uart_driver({{"name", "sunxi-uart"}, {"compat", "arm,uart-sunxi,t3"}}, sunxi_uart_api);
  driver_id = sunxi_uart_driver.init(0, nullptr);
  return driver_id;
}

driver_entry_level_default(sunxi_uart_entry);
driver_entry_level_untest(sunxi_uart_test_bind);
