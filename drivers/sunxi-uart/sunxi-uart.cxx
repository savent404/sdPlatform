#include <sunxi-uart.h>

#include <platform/drivers/uart.hxx>
#include <platform/entry.hxx>

// headers only for test
#include <platform.h>
#include <requirements.h>

#include <platform/bits.hxx>
#include <platform/device.hxx>

using namespace platform::drivers::uart;  // NOLINT

static int api_setup(runtime_ptr rt);
static int api_tx(runtime_ptr rt, const char *buf, size_t len);
static int api_config_parity(runtime_ptr rt);
static int api_config_data_bit(runtime_ptr rt);
static int api_config_stop_bit(runtime_ptr rt);
static int api_config_baud_rate(runtime_ptr rt);

driver* sunxi_uart_driver = nullptr;

extern "C" int sunxi_uart_entry() {
  platform::entry::platform_init();
  api sunxi_uart_api = {
      .match = nullptr,
      .setup = api_setup,
      .shutdown = nullptr,
      .tx = api_tx,
      .stop_rx = nullptr,
      .pm = nullptr,
      .config_parity = api_config_parity,
      .config_data_bit = api_config_data_bit,
      .config_stop_bit = api_config_stop_bit,
      .config_baud_rate = api_config_baud_rate,
      .ioctl = nullptr,
  };
  sunxi_uart_driver = new driver({{"name", "sunxi-uart"}, {"compat", "arm,uart-sunxi,t3"}}, sunxi_uart_api);
  return sunxi_uart_driver->init(0, nullptr);
}

driver_entry_level_default(sunxi_uart_entry);

int api_setup(runtime_ptr rt) {
  int res = 0;

  /* enable fifo for rx/tx */
  auto pfcr = platform::bits::io_addr_shift<int8_t>(rt->mem_base, SUNXI_UART_FCR);
  platform::bits::out(pfcr, SUNXI_UART_FCR_RXTRG_1_2 | SUNXI_UART_FCR_TXTRG_1_2 | SUNXI_UART_FCR_FIFO_EN);

  res = api_config_data_bit(rt);
  if (res) goto out;

  res = api_config_parity(rt);
  if (res) goto out;

  res = api_config_stop_bit(rt);
  if (res) goto out;

  res = api_config_baud_rate(rt);
  if (res) goto out;

out:
  return res;
}

int api_config_parity(runtime_ptr rt) {
  uint32_t* plcr = platform::bits::io_addr_shift<uint32_t>(rt->mem_base, SUNXI_UART_LCR);
  uint32_t lcr = *plcr;

  // pairty
  switch (rt->parity) {
    case 0:
      lcr = platform::bits::clear_bit(lcr, 3);
      break;
    case 1:  // odd
      lcr = platform::bits::set_bit(lcr, 3);
      lcr = platform::bits::clear_bit(lcr, 4);
      break;
    case 2:  // even
      lcr = platform::bits::set_bit(lcr, 3);
      lcr = platform::bits::clear_bit(lcr, 4);
      break;
    default:  // as parameter invalid
      return eno::ENO_INVALID;
  }
  platform::bits::out(plcr, lcr);
  return 0;
}

static int api_config_data_bit(runtime_ptr rt) {
  uint32_t* plcr = platform::bits::io_addr_shift<uint32_t>(rt->mem_base, SUNXI_UART_LCR);
  uint32_t lcr = *plcr;
  switch (rt->data_bits) {
    case 5:  // 5Bit - DLS(00)
      lcr = platform::bits::clear_bit(lcr, 0);
      lcr = platform::bits::clear_bit(lcr, 1);
      break;
    case 6:  // 6Bit - DLS(01)
      lcr = platform::bits::set_bit(lcr, 0);
      lcr = platform::bits::clear_bit(lcr, 1);
      break;
    case 7:  // 7Bit - DLS(10)
      lcr = platform::bits::clear_bit(lcr, 0);
      lcr = platform::bits::set_bit(lcr, 1);
      break;
    case 8:
      lcr = platform::bits::set_bit(lcr, 1);
      lcr = platform::bits::set_bit(lcr, 1);
      break;
    default: return eno::ENO_INVALID;
  }
  platform::bits::out(plcr, lcr);
  return 0;
}

int api_config_stop_bit(runtime_ptr rt) {
  uint32_t* plcr = platform::bits::io_addr_shift<uint32_t>(rt->mem_base, SUNXI_UART_LCR);
  uint32_t lcr = platform::bits::in(plcr);

  switch (rt->stop_bits) {
    case 0:  // 1 bit
      lcr = platform::bits::clear_bit(lcr, 2);
      break;
    case 1:  // 1.5 bit
      // as sunxi-uart datasheet, 1.5 stop bit when DLS is zero
      if (platform::bits::and_bit(lcr, (uint32_t)0x03) != 0) return eno::ENO_INVALID;
      lcr = platform::bits::set_bit(lcr, 2);
      break;
    case 2:
      // as sunxi-uart datasheet, 1.5 stop bit when DLS is zero
      if (platform::bits::and_bit(lcr, (uint32_t)0x03) == 0) return eno::ENO_INVALID;
      lcr = platform::bits::set_bit(lcr, 2);
      break;
    default:
      return eno::ENO_INVALID;
  }
  platform::bits::out(plcr, lcr);
  return 0;
}

int api_config_baud_rate(runtime_ptr rt) {
  // check ccn(rcc) find out APB2's freq
  uint8_t *plcr = platform::bits::io_addr_shift<uint8_t>(rt->mem_base, SUNXI_UART_LCR);
  uint8_t *pdll = platform::bits::io_addr_shift<uint8_t>(rt->mem_base, SUNXI_UART_DLL);
  uint8_t *pdlh = platform::bits::io_addr_shift<uint8_t>(rt->mem_base, SUNXI_UART_DLH);
  uint8_t *phalt= platform::bits::io_addr_shift<uint8_t>(rt->mem_base, SUNXI_UART_HALT);
  size_t APB2_freq = 24 * 1000 * 1000; // 24MHz
  size_t baud_rate = rt->baudrate;
  uint16_t dvi = 0;
  size_t res = APB2_freq / (16 * baud_rate);
  dvi = static_cast<uint16_t>(res);

  /* hold tx so that uart will update lcr and baud in the gap of rx */
  uint8_t halt = platform::bits::in(phalt);
  halt = platform::bits::clear_bit(halt, 0);
  halt = platform::bits::clear_bit(halt, 1);
  platform::bits::out(phalt, halt | SUNXI_UART_HALT_FORCECFG | SUNXI_UART_HALT_HTX);

  uint8_t lcr = platform::bits::in(plcr);
  lcr = platform::bits::clear_bit(lcr, 7);
  platform::bits::out(plcr, lcr | SUNXI_UART_LCR_DLAB);
  platform::bits::out(pdll, dvi & 0xFF);
  platform::bits::out(pdlh, dvi >> 8);
  platform::bits::out(phalt, halt | SUNXI_UART_HALT_HTX | SUNXI_UART_HALT_FORCECFG | SUNXI_UART_HALT_LCRUP);
  do {
    halt = platform::bits::in(phalt);
  } while (halt & SUNXI_UART_HALT_LCRUP);

  /* cleanup halt */
  platform::bits::out(plcr, lcr);
  platform::bits::out(phalt, SUNXI_UART_HALT_FORCECFG);
  return 0;
}

int api_tx(runtime_ptr rt, const char* out, size_t len) {
  uint32_t* thr = platform::bits::io_addr_shift<uint32_t>(rt->mem_base, SUNXI_UART_THR);
  uint32_t* lsr = platform::bits::io_addr_shift<uint32_t>(rt->mem_base, SUNXI_UART_LSR);
  uint32_t x;

  while (len--) {
    do {
      x = platform::bits::in(lsr);
    } while (platform::bits::get_bit(*lsr, 6) == 0);
    x = *out++;
    platform::bits::out(thr, x);
  }
  return eno::ENO_OK;
}

