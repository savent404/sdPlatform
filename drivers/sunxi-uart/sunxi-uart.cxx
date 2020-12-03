/**
 * @file sunxi-uart.cxx
 * @author savent (savent_gate@outlook.com)
 * @brief
 * @version 0.1
 * @date 2020-12-01
 *
 * Copyright 2020 jrlc
 *
 */
#include <loop.h>
#include <osal.h>
#include <sunxi-uart.h>

#include <memory>
#include <utility>

// clang-format off
#include <platform/bits.hxx>
#include <platform/debug.hxx>
#include <platform/drivers/uart.hxx>
#include <platform/entry.hxx>
// clang-format on

extern "C" void* sunxi_uart_irqipc_loop(void*);
extern "C" s32 sunxi_uart_irq_entry(irq_waiter_t*);

namespace platform::drivers::uart::sunxi_t3 {

struct runtime : public platform::runtime {
  RUNTIME_INIT(sunxi_t3_uart);
  runtime() : platform::runtime(), irq(0), irq_handle(0) { __init(); }

  int irq;
  ipc_handle_t irq_handle;
};

using platform::bits;
using platform::debug;

void setup_clk(void* ccu_base, int uart_index) {
  /* reset uart */
  static uint32_t* ccu_sw_reset_reg4 = bits::shift_addr<uint32_t*>(ccu_base, 0x2d8);
  auto sw_reset4 = bits::in(ccu_sw_reset_reg4);
  sw_reset4 = bits::clear_bits(sw_reset4, 16 + uart_index);
  bits::out(ccu_sw_reset_reg4, sw_reset4);
  sw_reset4 = bits::set_bits(sw_reset4, 16 + uart_index);
  bits::out(ccu_sw_reset_reg4, sw_reset4);

  /* enable gate */
  static uint32_t* ccu_bcg_reg3 = bits::shift_addr<uint32_t*>(ccu_base, 0x6c);
  auto bcg_reg3 = bits::in(ccu_bcg_reg3);
  bcg_reg3 = bits::clear_bits(bcg_reg3, 16 + uart_index);
  bits::out(ccu_bcg_reg3, bcg_reg3);
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
      auto tx_pull_reg = bits::shift_addr<uint32_t*>(pio_base, 0x140);
      auto tx_conf = bits::in(tx_conf_reg);
      tx_conf = bits::modify_bits(tx_conf, 0x03, 8, 3);
      bits::out(tx_conf_reg, tx_conf);
      auto tx_pull = bits::in(tx_pull_reg);
      tx_pull = bits::modify_bits(tx_pull, 0x01, 4, 2);
      bits::out(tx_pull_reg, tx_pull);
      /* UART_2 rx */
      auto rx_conf_reg = bits::shift_addr<uint32_t*>(pio_base, 0x128);
      auto rx_pull_reg = bits::shift_addr<uint32_t*>(pio_base, 0x140);
      auto rx_conf = bits::in(rx_conf_reg);
      rx_conf = bits::modify_bits(rx_conf, 0x03, 12, 3);
      bits::out(rx_conf_reg, rx_conf);
      auto rx_pull = bits::in(rx_pull_reg);
      rx_pull = bits::modify_bits(rx_pull, 0x01, 6, 2);
      bits::out(rx_pull_reg, rx_pull);
    } break;
    default:
      break;
  }
}

void handle_irq_rx(runtime_ptr rt, uint32_t lsr) {
  char ch = 0;
  int cnt = 255;
  auto reg_lsr = bits::shift_addr<uint32_t*>(rt->mem_base, SUNXI_UART_LSR);
  auto reg_rbr = bits::shift_addr<uint32_t*>(rt->mem_base, SUNXI_UART_RBR);
  do {
    if (lsr & SUNXI_UART_LSR_BRK_ERROR_BITS) {
      // Do nothing
    } else if ((lsr & SUNXI_UART_LSR_DR)) {
      ch = bits::in(reg_rbr)&0xFF;
      rt->rx_buffer.push(ch);
    }

    lsr = bits::in(reg_lsr);
  } while ((lsr & (SUNXI_UART_LSR_DR | SUNXI_UART_LSR_BI)) && cnt--);
}

void handle_irq_modem_status(runtime_ptr rt) {
  auto reg_msr = bits::shift_addr<uint32_t*>(rt->mem_base, SUNXI_UART_MSR);
  auto msr = bits::in(reg_msr);
  msr = msr;
}

void tx_switch(runtime_ptr rt, bool enable) {
  auto reg_ier = bits::shift_addr<uint32_t*>(rt->mem_base, SUNXI_UART_IER);
  auto ier = bits::in(reg_ier);
  if (enable)
    ier = bits::set_bits(ier, 1);
  else
    ier = bits::clear_bits(ier, 1);
  bits::out(reg_ier, ier);
}

void rx_switch(runtime_ptr rt, bool enable) {
  auto reg_ier = bits::shift_addr<uint32_t*>(rt->mem_base, SUNXI_UART_IER);
  auto ier = bits::in(reg_ier);
  if (enable)
    ier = bits::set_bits(ier, 0);
  else
    ier = bits::clear_bits(ier, 0);
  bits::out(reg_ier, ier);
}

void handle_irq_tx(runtime_ptr rt, uint32_t lsr) {
  static const int fifo_left = 32;
  size_t cnt = rt->tx_buffer.size();
  cnt = cnt > fifo_left ? fifo_left : cnt;
  auto reg_thr = bits::shift_addr<char*>(rt->mem_base, SUNXI_UART_THR);
  auto& buffer = rt->tx_buffer;

  if (!cnt) {
    // disable tx interrupt
    tx_switch(rt, false);
    return;
  }
  while (cnt--) {
    bits::out(reg_thr, buffer.front());
    buffer.pop();
  }
  if ((lsr & SUNXI_UART_LSR_TEMT) && !rt->tx_buffer.empty()) {
    // enable tx interrupt
    tx_switch(rt, true);
  }
}

int handle_irq(runtime_ptr rt) {
  auto reg_iir = bits::shift_addr<uint32_t*>(rt->mem_base, SUNXI_UART_IIR);
  auto reg_lsr = bits::shift_addr<uint32_t*>(rt->mem_base, SUNXI_UART_LSR);

  auto iir = bits::in(reg_iir)&0x0F;
  auto lsr = bits::in(reg_lsr);

  if (iir == SUNXI_UART_IIR_IID_BUSBSY) {
    /**
     * Before reseting lcr, we should ensure that uart is not in busy state.
     * Otherwise, a new busy interrupt will be introduced.
     * It is wise to set uart into loopback mode, since it can cut down the
     * serial in, then we should reset fifo(in some case, busy state can't be cleard until the fifo is empty)
     *
     */
    auto reg_mcr = bits::shift_addr<uint32_t*>(rt->mem_base, SUNXI_UART_MCR);
    auto reg_lcr = bits::shift_addr<uint32_t*>(rt->mem_base, SUNXI_UART_LCR);
    auto reg_fcr = bits::shift_addr<uint32_t*>(rt->mem_base, SUNXI_UART_FCR);
    auto reg_usr = bits::shift_addr<uint32_t*>(rt->mem_base, SUNXI_UART_USR);
    auto mcr = bits::in(reg_mcr);
    auto lcr = bits::in(reg_lcr);
    auto fcr = bits::in(reg_fcr);
    bits::out(reg_mcr, mcr | SUNXI_UART_MCR_LOOP);
    auto usr = bits::in(reg_usr);
    usr = usr;
    if (fcr & SUNXI_UART_FCR_FIFO_EN) {
      bits::out(reg_fcr, SUNXI_UART_FCR_FIFO_EN);
      bits::out(reg_fcr, SUNXI_UART_FCR_FIFO_EN | SUNXI_UART_FCR_TXFIFO_RST | SUNXI_UART_FCR_RXFIFO_RST);
      bits::out(reg_fcr, 0);
    }
    bits::out(reg_fcr, fcr);
    bits::out(reg_lcr, lcr);
    bits::out(reg_mcr, mcr);
  } else {
    if (lsr & (SUNXI_UART_LSR_DR | SUNXI_UART_LSR_BI)) {
      handle_irq_rx(rt, lsr);
    } else if (iir & SUNXI_UART_IIR_IID_CHARTO) {
      /** has charto irq but no dr lsr ? just read and ignore */
      auto reg_rbr = bits::shift_addr<uint32_t*>(rt->mem_base, SUNXI_UART_RBR);
      bits::in(reg_rbr);
    }
    handle_irq_modem_status(rt);
    if (iir == SUNXI_UART_IIR_IID_THREMP || ((lsr & SUNXI_UART_LSR_TEMT) && !rt->tx_buffer.empty()))
      handle_irq_tx(rt, lsr);
  }

  return 0;
}

s32 sunxi_uart_irq_entry(irq_waiter_t* info) {
  auto rt = reinterpret_cast<runtime_ptr>(info->data);
  auto pri_rt = rt->pri_rt->promote<runtime>();
  if (pri_rt->irq != static_cast<int>(info->irq)) return -1;
  return handle_irq(rt);
}

void* __attribute__((noreturn)) sunxi_uart_irqipc_loop(void* ptr) {
  static const int irq_index[] = {
      33,  // uart 0
      34,  // uart 1
      35,  // uart 2
      36,  // uart 3
      49,  // uart 4
      50,  // uart 5
      51,  // uart 6
      52,  // uart 7
  };
  runtime_ptr rt = reinterpret_cast<runtime_ptr>(ptr);
  auto pri_rt = rt->pri_rt->promote<runtime>();

  auto loop = loop_new();
  debug::assert(loop);
  debug::assert(rt->uart_idx < (sizeof(irq_index) / sizeof(irq_index[0])));

  pri_rt->irq = irq_index[rt->uart_idx];
  auto waiter = loop_irq_waiter_new(pri_rt->irq, sunxi_uart_irq_entry, rt);
  debug::assert(waiter);

  loop_irq_waiter_register(loop, waiter);

  loop_run(loop);
  return NULL;
}

void sunxi_uart_irq_entry(void* ptr) {
  runtime_ptr rt = reinterpret_cast<runtime_ptr>(ptr);
  handle_irq(rt);
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
      lcr = bits::modify_bits(lcr, 0x01, 3, 2);
      break;
    case 2:  // even
      lcr = bits::modify_bits(lcr, 0x03, 3, 2);
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
      lcr = bits::clear_bits(lcr, 0, 2);
      break;
    case 6:  // 6Bit - DLS(01)
      lcr = bits::modify_bits(lcr, 0x01, 0, 2);
      break;
    case 7:  // 7Bit - DLS(10)
      lcr = bits::modify_bits(lcr, 0x02, 0, 2);
      break;
    case 8:  // 8Bit - DLS(11)
      lcr = bits::modify_bits(lcr, 0x03, 0, 2);
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
      if (bits::get_bits(lcr, 0, 2) != 0) return eno::ENO_INVALID;
      lcr = bits::set_bits(lcr, 2);
      break;
    case 2:
      // as sunxi-uart datasheet, 1.5 stop bit when DLS is zero
      if (bits::get_bits(lcr, 0, 2) == 0) return eno::ENO_INVALID;
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
  uint32_t* plcr = bits::shift_addr<uint32_t*>(rt->mem_base, SUNXI_UART_LCR);
  uint32_t* pdll = bits::shift_addr<uint32_t*>(rt->mem_base, SUNXI_UART_DLL);
  uint32_t* pdlh = bits::shift_addr<uint32_t*>(rt->mem_base, SUNXI_UART_DLH);
  uint32_t* phalt = bits::shift_addr<uint32_t*>(rt->mem_base, SUNXI_UART_HALT);
  size_t APB2_freq = 24 * 1000 * 1000;  // 24MHz
  size_t baud_rate = rt->baudrate;
  uint16_t dvi = 0;
  size_t res = APB2_freq / (16 * baud_rate);
  dvi = static_cast<uint16_t>(res);

  /* hold tx so that uart will update lcr and baud in the gap of rx */
  uint32_t halt = bits::in(phalt);
  halt = bits::clear_bits(halt, 0, 2);
  bits::out(phalt, halt | SUNXI_UART_HALT_FORCECFG | SUNXI_UART_HALT_HTX);

  uint32_t lcr = bits::in(plcr);
  lcr = bits::clear_bits(lcr, 7);
  bits::out(plcr, lcr | SUNXI_UART_LCR_DLAB);
  bits::out(pdll, bits::and_bits(dvi, 0xFF));
  bits::out(pdlh, bits::shift_bits(dvi, -8));
  bits::out(phalt, halt | SUNXI_UART_HALT_HTX | SUNXI_UART_HALT_FORCECFG | SUNXI_UART_HALT_LCRUP);
  do {
    halt = bits::in(phalt);
  } while (halt & SUNXI_UART_HALT_LCRUP);

  /* cleanup halt */
  bits::out(plcr, lcr);
  bits::out(phalt, SUNXI_UART_HALT_FORCECFG);
  return 0;
}

int api_start_tx(runtime_ptr rt) {
  tx_switch(rt, true);
  return eno::ENO_OK;
}

int api_stop_tx(runtime_ptr rt) {
  tx_switch(rt, false);
  return eno::ENO_OK;
}

int api_start_rx(runtime_ptr rt) {
  rx_switch(rt, true);
  return eno::ENO_OK;
}
int api_stop_rx(runtime_ptr rt) {
  rx_switch(rt, false);
  return eno::ENO_OK;
}
int api_poll_rx(runtime_ptr rt, char* pc, size_t len) {
  auto reg_lsr = bits::shift_addr<uint32_t*>(rt->mem_base, SUNXI_UART_LSR);
  auto reg_rbr = bits::shift_addr<char*>(rt->mem_base, SUNXI_UART_RBR);
  auto lsr = bits::in(reg_lsr);
  int i = 0;

  while (len-- && bits::and_bits(lsr, SUNXI_UART_LSR_DR)) {
    *pc++ = bits::in(reg_rbr);
    i++;
    lsr = bits::in(reg_lsr);
  }
  return i;
}

int api_poll_tx(runtime_ptr rt) {
  auto& buff = rt->tx_buffer;
  auto reg_lsr = bits::shift_addr<uint32_t*>(rt->mem_base, SUNXI_UART_LSR);
  auto reg_thr = bits::shift_addr<char*>(rt->mem_base, SUNXI_UART_THR);
  uint32_t lsr;
  while (!buff.empty()) {
    do {
      lsr = bits::in(reg_lsr);
    } while ((lsr & SUNXI_UART_LSR_TEMT) == 0);
    bits::out(reg_thr, buff.front());
    buff.pop();
  }
  return eno::ENO_OK;
}

int api_shutdown(runtime_ptr rt) {
  // no need close irq handler
  return eno::ENO_OK;
}

int api_setup(runtime_ptr rt) {
  int res = 0;
  static void* ccu_base = reinterpret_cast<void*>(0x38001000);

  /* malloc for privatte runtime parameter */
  if (!rt->pri_rt.get()) {
    auto pri_rt = new runtime;
    rt->pri_rt = std::move(std::unique_ptr<platform::runtime>(pri_rt));
  }

  /* enable ccu for uart */
  setup_clk(ccu_base, rt->uart_idx);

  /* pinmux for uart */
  setup_pinmux(rt->uart_idx);

  /* enable fifo for rx/tx */
  auto pfcr = bits::shift_addr<uint32_t*>(rt->mem_base, SUNXI_UART_FCR);
  bits::out(pfcr, SUNXI_UART_FCR_RXTRG_1CH | SUNXI_UART_FCR_TXTRG_1_2 | SUNXI_UART_FCR_FIFO_EN);

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
    bits::out(ier_reg, 0x8c);  // disable DR irq
  }
  /* set MCR */
  {
    auto mcr_reg = bits::shift_addr<uint32_t*>(rt->mem_base, SUNXI_UART_MCR);
    bits::out(mcr_reg, 0x03);
  }

  /* setup interrupt */
  {
    auto pri_rt = rt->pri_rt->promote<runtime>();
    if (!pri_rt->irq_handle) pri_rt->irq_handle = os.os_thread_create(sunxi_uart_irqipc_loop, rt);
    if (!pri_rt->irq_handle) return eno::ENO_SYSCALL_ERR;
  }
out:
  return res;
}

}  // namespace platform::drivers::uart::sunxi_t3

platform::drivers::uart::driver* sunxi_uart_driver = nullptr;

extern "C" int sunxi_uart_entry(void* env) {
  platform::entry::platform_init(env);
  platform::drivers::uart::api sunxi_uart_api = {
      .match = nullptr,
      .setup = platform::drivers::uart::sunxi_t3::api_setup,
      .shutdown = platform::drivers::uart::sunxi_t3::api_shutdown,
      .start_tx = platform::drivers::uart::sunxi_t3::api_start_tx,
      .stop_tx = platform::drivers::uart::sunxi_t3::api_stop_tx,
      .poll_tx = platform::drivers::uart::sunxi_t3::api_poll_tx,
      .start_rx = platform::drivers::uart::sunxi_t3::api_start_rx,
      .stop_rx = platform::drivers::uart::sunxi_t3::api_stop_rx,
      .poll_rx = platform::drivers::uart::sunxi_t3::api_poll_rx,
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

driver_entry(sunxi_uart_entry, "sunxi-uart", "arm,uart-sunxi,t3");
