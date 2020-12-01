#include <sunxi-uart.h>

#include <platform/bits.hxx>
#include <platform/drivers/uart.hxx>
#include <platform/entry.hxx>

namespace platform::drivers::uart::sunxi_t3 {

using platform::bits;

void setup_clk(void* ccu_base, int uart_index) {
  /* reset uart */
  static uint32_t* ccu_sw_reset_reg4 = bits::shift_addr<uint32_t*>(ccu_base, 0x2d8);
  auto sw_reset4 = bits::in(ccu_sw_reset_reg4);
  sw_reset4 = bits::clear_bits(sw_reset4, 16 + uart_index);
  bits::out(ccu_sw_reset_reg4, sw_reset4);
  // for (int i = 0; i < 100; i++) {
  // }
  sw_reset4 = bits::set_bits(sw_reset4, 16 + uart_index);
  bits::out(ccu_sw_reset_reg4, sw_reset4);

  /* enable gate */
  static uint32_t* ccu_bcg_reg3 = bits::shift_addr<uint32_t*>(ccu_base, 0x6c);
  auto bcg_reg3 = bits::in(ccu_bcg_reg3);
  bcg_reg3 = bits::clear_bits(bcg_reg3, 16 + uart_index);
  bits::out(ccu_bcg_reg3, bcg_reg3);
  // for (int i = 0; i < 100; i++) {
  // }
  bcg_reg3 = bits::set_bits(bcg_reg3, 16 + uart_index);
  bits::out(ccu_bcg_reg3, bcg_reg3);
}

/**
 * @brief setup uart's pinmux
 *
 * @param uart_index uart's index(start at zero(0))
 * @note  use gpio's function and set pull as pull-up mode
 */
void setup_pinmux(int uart_index) {
  constexpr auto pio_base = (0x38001800);
  switch (uart_index) {
    case 2: {
      /* UART_2 tx */
      auto tx_conf_reg = bits::shift_addr<uint32_t*>(pio_base, 0x128);
      auto tx_pull_reg = bits::shift_addr<uint32_t*>(pio_base, 0x128);
      auto tx_conf = bits::in(tx_conf_reg);
      tx_conf = bits::clear_bits(tx_conf, 8, 3);
      tx_conf = bits::or_bits(tx_conf, bits::shift_bits(0x03, 8));
      bits::out(tx_conf_reg, tx_conf);
      auto tx_pull = bits::in(tx_pull_reg);
      tx_pull &= ~(0x3 << 2 * 2);
      tx_pull |= 0x1 << 2 * 2;
      bits::out(tx_pull_reg, tx_pull);
      /* UART_2 rx */
      auto rx_conf_reg = bits::shift_addr<uint32_t*>(pio_base, 0x128);
      auto rx_pull_reg = bits::shift_addr<uint32_t*>(pio_base, 0x128);
      auto rx_conf = bits::in(rx_conf_reg);
      rx_conf &= ~(0x07 << 12);
      rx_conf |= 0x3 << 12;
      bits::out(rx_conf_reg, rx_conf);
      auto rx_pull = bits::in(rx_pull_reg);
      rx_pull &= ~(0x3 << 2 * 3);
      rx_pull |= 0x1 << 2 * 3;
      bits::out(rx_pull_reg, rx_pull);
    } break;
    default:
      break;
  }
}

int api_config_parity(runtime_ptr rt) {
  uint32_t* plcr = bits::shift_addr<uint32_t*>(rt->mem_base, SUNXI_UART_LCR);
  uint32_t lcr = *plcr;

  // pairty
  switch (rt->parity) {
    case 0:
      lcr = bits::clear_bits(lcr, 3);
      break;
    case 1:  // odd
      lcr = bits::set_bits(lcr, 3);
      lcr = bits::clear_bits(lcr, 4);
      break;
    case 2:  // even
      lcr = bits::set_bits(lcr, 3);
      lcr = bits::clear_bits(lcr, 4);
      break;
    default:  // as parameter invalid
      return eno::ENO_INVALID;
  }
  bits::out(plcr, lcr);
  return 0;
}

int api_config_data_bit(runtime_ptr rt) {
  uint32_t* plcr = bits::shift_addr<uint32_t*>(rt->mem_base, SUNXI_UART_LCR);
  uint32_t lcr = *plcr;
  switch (rt->data_bits) {
    case 5:  // 5Bit - DLS(00)
      lcr = bits::clear_bits(lcr, 0);
      lcr = bits::clear_bits(lcr, 1);
      break;
    case 6:  // 6Bit - DLS(01)
      lcr = bits::set_bits(lcr, 0);
      lcr = bits::clear_bits(lcr, 1);
      break;
    case 7:  // 7Bit - DLS(10)
      lcr = bits::clear_bits(lcr, 0);
      lcr = bits::set_bits(lcr, 1);
      break;
    case 8:
      lcr = bits::set_bits(lcr, 0);
      lcr = bits::set_bits(lcr, 1);
      break;
    default:
      return eno::ENO_INVALID;
  }
  bits::out(plcr, lcr);
  return 0;
}

int api_config_stop_bit(runtime_ptr rt) {
  uint32_t* plcr = bits::shift_addr<uint32_t*>(rt->mem_base, SUNXI_UART_LCR);
  uint32_t lcr = bits::in(plcr);

  switch (rt->stop_bits) {
    case 0:  // 1 bit
      lcr = bits::clear_bits(lcr, 2);
      break;
    case 1:  // 1.5 bit
      // as sunxi-uart datasheet, 1.5 stop bit when DLS is zero
      if (bits::and_bits(lcr, (uint32_t)0x03) != 0) return eno::ENO_INVALID;
      lcr = bits::set_bits(lcr, 2);
      break;
    case 2:
      // as sunxi-uart datasheet, 1.5 stop bit when DLS is zero
      if (bits::and_bits(lcr, (uint32_t)0x03) == 0) return eno::ENO_INVALID;
      lcr = bits::set_bits(lcr, 2);
      break;
    default:
      return eno::ENO_INVALID;
  }
  bits::out(plcr, lcr);
  return 0;
}

int api_config_baud_rate(runtime_ptr rt) {
  // check ccn(rcc) find out APB2's freq
  uint8_t* plcr = bits::shift_addr<uint8_t*>(rt->mem_base, SUNXI_UART_LCR);
  uint8_t* pdll = bits::shift_addr<uint8_t*>(rt->mem_base, SUNXI_UART_DLL);
  uint8_t* pdlh = bits::shift_addr<uint8_t*>(rt->mem_base, SUNXI_UART_DLH);
  uint8_t* phalt = bits::shift_addr<uint8_t*>(rt->mem_base, SUNXI_UART_HALT);
  size_t APB2_freq = 24 * 1000 * 1000;  // 24MHz
  size_t baud_rate = rt->baudrate;
  uint16_t dvi = 0;
  size_t res = APB2_freq / (16 * baud_rate);
  dvi = static_cast<uint16_t>(res);

  /* hold tx so that uart will update lcr and baud in the gap of rx */
  uint8_t halt = bits::in(phalt);
  halt = bits::clear_bits(halt, 0);
  halt = bits::clear_bits(halt, 1);
  bits::out(phalt, halt | SUNXI_UART_HALT_FORCECFG | SUNXI_UART_HALT_HTX);

  uint8_t lcr = bits::in(plcr);
  lcr = bits::clear_bits(lcr, 7);
  bits::out(plcr, lcr | SUNXI_UART_LCR_DLAB);
  bits::out(pdll, dvi & 0xFF);
  bits::out(pdlh, dvi >> 8);
  bits::out(phalt, halt | SUNXI_UART_HALT_HTX | SUNXI_UART_HALT_FORCECFG | SUNXI_UART_HALT_LCRUP);
  do {
    halt = bits::in(phalt);
  } while (halt & SUNXI_UART_HALT_LCRUP);

  /* cleanup halt */
  bits::out(plcr, lcr);
  bits::out(phalt, SUNXI_UART_HALT_FORCECFG);
  return 0;
}

int api_tx(runtime_ptr rt, const char* out, size_t len) {
  uint32_t* thr = bits::shift_addr<uint32_t*>(rt->mem_base, SUNXI_UART_THR);
  uint32_t* lsr = bits::shift_addr<uint32_t*>(rt->mem_base, SUNXI_UART_LSR);
  uint32_t x;

  while (len--) {
    do {
      x = bits::in(lsr);
    } while (bits::get_bits(*lsr, 6) == 0);
    x = *out++;
    bits::out(thr, x);
  }
  return eno::ENO_OK;
}

int api_setup(runtime_ptr rt) {
  int res = 0;
  static void* ccu_base = reinterpret_cast<void*>(0x38001000);

  /* enable ccu for uart */
  setup_clk(ccu_base, rt->uart_idx);

  /* pinmux for uart */
  setup_pinmux(rt->uart_idx);

  /* enable fifo for rx/tx */
  auto pfcr = bits::shift_addr<int8_t*>(rt->mem_base, SUNXI_UART_FCR);
  bits::out(pfcr, SUNXI_UART_FCR_RXTRG_1_2 | SUNXI_UART_FCR_TXTRG_1_2 | SUNXI_UART_FCR_FIFO_EN);

  /* config data bit length */
  res = api_config_data_bit(rt);
  if (res) goto out;

  /* config parity */
  res = api_config_parity(rt);
  if (res) goto out;

  /* config stop bit length */
  res = api_config_stop_bit(rt);
  if (res) goto out;

  /* config uart baud rate */
  res = api_config_baud_rate(rt);
  if (res) goto out;

  /**
   * @brief enable PTIME | EDSSI | ELSI | ERBFI
   * 
   * @note  PTIME:  THRE interrupt
   *        EDSSI:  Modem status interrupt
   *        ELSI:   Receiver Line interrupt
   *        ERBFI:  Received data interrupt
   */
  {
    auto ier_reg = bits::shift_addr<uint32_t*>(rt->mem_base, SUNXI_UART_IER);
    bits::out(ier_reg, 0x83);
  }
  /* set MCR */
  {
    auto mcr_reg = bits::shift_addr<uint32_t*>(rt->mem_base, SUNXI_UART_MCR);
    bits::out(mcr_reg, 0x03);
  }

out:
  return res;
}

}  // namespace platform::drivers::uart::sunxi_t3

platform::drivers::uart::driver* sunxi_uart_driver = nullptr;

extern "C" int sunxi_uart_entry() {
  platform::entry::platform_init();
  platform::drivers::uart::api sunxi_uart_api = {
      .match = nullptr,
      .setup = platform::drivers::uart::sunxi_t3::api_setup,
      .shutdown = nullptr,
      .tx = platform::drivers::uart::sunxi_t3::api_tx,
      .stop_rx = nullptr,
      .pm = nullptr,
      .config_parity = platform::drivers::uart::sunxi_t3::api_config_parity,
      .config_data_bit = platform::drivers::uart::sunxi_t3::api_config_data_bit,
      .config_stop_bit = platform::drivers::uart::sunxi_t3::api_config_stop_bit,
      .config_baud_rate = platform::drivers::uart::sunxi_t3::api_config_baud_rate,
      .ioctl = nullptr,
  };
  sunxi_uart_driver =
      new platform::drivers::uart::driver({{"name", "sunxi-uart"}, {"compat", "arm,uart-sunxi,t3"}}, sunxi_uart_api);
  return sunxi_uart_driver->init(0, nullptr);
}

driver_entry_level_default(sunxi_uart_entry);
