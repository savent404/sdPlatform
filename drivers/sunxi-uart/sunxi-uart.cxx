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
  uint32_t* thr = platform::bits::io_addr_shift<uint32_t>(rt->mem_base, SUNXI_UART_THR);
  uint32_t* lsr = platform::bits::io_addr_shift<uint32_t>(rt->mem_base, SUNXI_UART_LSR);
  uint32_t x;

  while (len--) {
    do {
      x = platform::bits::in(lsr);
    } while (x & SUNXI_UART_LSR_TEMT == 0);
    x = *out++;
    platform::bits::out(thr, x);
  }
  return eno::ENO_OK;
}

driver* sunxi_uart_driver = nullptr;

extern "C" int sunxi_uart_entry() {
  platform::entry::platform_init();
  api sunxi_uart_api = {
      .match = nullptr,
      .setup = sunxi_uart_setup,
      .shutdown = nullptr,
      .tx = sunxi_uart_tx,
  };
  sunxi_uart_driver = new driver({{"name", "sunxi-uart"}, {"compat", "arm,uart-sunxi,t3"}}, sunxi_uart_api);
  return sunxi_uart_driver->init(0, nullptr);
}

driver_entry_level_default(sunxi_uart_entry);
