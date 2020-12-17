/**
 * @file uart.cxx
 * @author savent (savent_gate@outlook.com)
 * @brief
 * @version 0.1
 * @date 2020-12-02
 *
 * Copyright 2020 jrlc
 *
 */
#include <platform-types.h>

#include <platform/bits.hxx>
#include <platform/drivers/uart.hxx>
#include <platform/utils.hxx>

namespace platform::drivers::uart {

driver::driver(initial_list list, const api& api)
    : platform::driver(list, std::unique_ptr<platform::runtime>(nullptr)) {
  memcpy(&api_, &api, sizeof(api_));
}
driver::~driver() {}

int driver::init_(int argc, char** argv) { return eno::ENO_OK; }

int driver::deinit_() {
  // TODO(savent): delete all device then u can leave
  return eno::ENO_NOPERMIT;
}

int driver::bind_(device_ref dev) {
  // 默认返回 INVALID
  int res = eno::ENO_INVALID;

  // 从device的config字段解析runtime参数
  runtime tmp_rt;
  if (!load_device_runtime(dev.get_config(), &tmp_rt)) return eno::ENO_INVALID;

  // 如果硬件驱动包含匹配检查，执行检查
  if (api_.match) {
    res = api_.match(&tmp_rt);
    if (res) return res;
  }

  // 更新device的运行时
  auto rt_ptr = new runtime(std::move(tmp_rt));
  rt_ptr->file_flag = 0;
  if (!rt_ptr) return eno::ENO_NOMEM;
  dev.set_runtime(device::runtime_ptr(rt_ptr));

  res = eno::ENO_OK;
  return res;
}

int driver::unbind_(device_ref dev) { return eno::ENO_NOPERMIT; }

int driver::open_(device_ref dev, int flags) {
  int res = eno::ENO_OK;
  auto rt = dev.get_runtime().promote<runtime>();

  if (rt->file_flag) return eno::ENO_BUSY;

  if (flags & fflag::FF_READ) {
    if (flags & fflag::FF_POLL && !api_.poll_rx) return eno::ENO_NOTIMPL;
    if (!(flags & fflag::FF_POLL) && (!api_.start_rx || !api_.stop_rx)) return eno::ENO_NOTIMPL;
  }
  if (flags & fflag::FF_WRITE) {
    if (flags & fflag::FF_POLL && !api_.poll_tx) return eno::ENO_NOTIMPL;
    if (!(flags & fflag::FF_POLL) && (!api_.start_tx || !api_.stop_tx)) return eno::ENO_NOTIMPL;
  }
  if (!api_.setup || !api_.shutdown) return eno::ENO_NOTIMPL;

  /* call apis */
  res = api_.setup(rt);
  if (res) goto out;

  // enable recv in no-block mode
  if ((flags & fflag::FF_READ) && !(flags & fflag::FF_POLL)) {
    res = api_.start_rx(rt);
    if (res) goto out;
  }
  rt->file_flag = flags;
out:
  return res;
}

int driver::close_(device_ref dev) {
  auto rt = dev.get_runtime().promote<runtime>();
  if (!rt->file_flag) return eno::ENO_OK;

  int res = api_.shutdown(rt);
  if (!res) rt->file_flag = 0;

  return res;
}

int driver::transfer_(device_ref dev, const void* in, size_t in_len, void* out, size_t out_len) {
  return eno::ENO_NOPERMIT;
}

int driver::write_(device_ref dev, const void* out, size_t len) {
  runtime* rt = dev.get_runtime().promote<runtime>();
  auto& buf = rt->tx_buffer;
  const char* ptr = reinterpret_cast<const char*>(out);

  if (!bits::and_bits(rt->file_flag, fflag::FF_WRITE)) return eno::ENO_INVALID;

  for (size_t i = 0; i < len; i++) {
    buf->push(*ptr++);
  }
  int res = 0;
  if (rt->file_flag & fflag::FF_POLL) {
    res = api_.poll_tx(rt);
  } else {
    res = api_.start_tx(rt);
  }
  if (res < 0) return res;
  return len;
}

int driver::read_(device_ref dev, void* in, size_t len) {
  char* ptr = reinterpret_cast<char*>(in);
  runtime* rt = dev.get_runtime().promote<runtime>();

  if (!bits::and_bits(rt->file_flag, fflag::FF_READ)) return eno::ENO_INVALID;

  if (rt->file_flag & fflag::FF_POLL) {
    return api_.poll_rx(rt, reinterpret_cast<char*>(in), len);
  } else {
    auto& rx_buf = rt->rx_buffer;
    auto left = rx_buf->size();
    auto cnt = left > len ? len : left;
    for (size_t i = 0; i < cnt; i++) {
      *ptr++ = rx_buf->front();
      rx_buf->pop();
    }
    return cnt;
  }
}

int driver::ioctl_(device_ref dev, int cmds, const void* in, size_t in_len, void* out, size_t* out_len, size_t max) {
  runtime* rt = dev.get_runtime().promote<runtime>();
  int res = eno::ENO_OK;
  switch (cmds) {
    case fcmds::FC_GET_BAUD_RATE: {
      res = tools::ioctl_helper_get_param(rt->baudrate, out, out_len, max);
    } break;
    case fcmds::FC_GET_PARITY_MODE: {
      res = tools::ioctl_helper_get_param(rt->parity, out, out_len, max);
    } break;
    case fcmds::FC_GET_STOP_BIT: {
      res = tools::ioctl_helper_get_param(rt->stop_bits, out, out_len, max);
    } break;
    case fcmds::FC_SET_BAUD_RATE: {
      if (!api_.config_baud_rate) return eno::ENO_NOTIMPL;
      auto origin = rt->baudrate;
      res = tools::ioctl_helper_set_param(&rt->baudrate, in, in_len);
      if (res) break;
      res = api_.config_baud_rate(rt);
      if (res) rt->baudrate = origin;
    } break;
    case fcmds::FC_SET_PARITY_MODE: {
      if (!api_.config_parity) return eno::ENO_NOTIMPL;
      auto origin = rt->parity;
      res = tools::ioctl_helper_set_param(&rt->parity, in, in_len);
      if (res) break;
      res = api_.config_parity(rt);
      if (res) rt->parity = origin;
    } break;
    case fcmds::FC_SET_STOP_BIT: {
      if (!api_.config_stop_bit) return eno::ENO_NOTIMPL;
      auto origin = rt->parity;
      res = tools::ioctl_helper_set_param(&rt->stop_bits, in, in_len);
      if (res) break;
      res = api_.config_stop_bit(rt);
      if (res) rt->parity = origin;
    } break;
    default:
      res = api_.ioctl ? api_.ioctl(rt, cmds, in, in_len, out, out_len, max) : eno::ENO_INVALID;
  }
  if (!res && IS_CONFIG_FCMDS(cmds)) {
    devmgr_update();
  }
  return eno::ENO_OK;
}

void driver::to_json_(cJSON* obj) { }
void driver::from_json_(cJSON*) {}
void driver::register_internal_syscall_() {}

#define CHECK_KEY(conf, key) \
  if (!conf.has(key)) return false;
#define GET_VAL(conf, key, internal_type, outside_type) (reinterpret_cast<outside_type>(conf.get<internal_type>(key)))

bool uart::driver::load_device_runtime(parameters_ref dev_conf, runtime* rt) {
  CHECK_KEY(dev_conf, "config/base");
  CHECK_KEY(dev_conf, "config/range");
  CHECK_KEY(dev_conf, "config/baudrate");
  CHECK_KEY(dev_conf, "config/parity");
  CHECK_KEY(dev_conf, "config/data_bits");
  CHECK_KEY(dev_conf, "config/stop_bits");
  CHECK_KEY(dev_conf, "config/uart_idx");

  rt->mem_base = GET_VAL(dev_conf, "config/base", int, void*);
  rt->mem_range = GET_VAL(dev_conf, "config/range", int, int);
  rt->baudrate = GET_VAL(dev_conf, "config/baudrate", int, int);
  const char* parity_str = GET_VAL(dev_conf, "config/parity", const char*, const char*);
  if (!strcmp(parity_str, "none"))
    rt->parity = 0;
  else if (!strcmp(parity_str, "odd"))
    rt->parity = 1;
  else if (!strcmp(parity_str, "even"))
    rt->parity = 2;
  else
    return false;
  rt->data_bits = GET_VAL(dev_conf, "config/data_bits", int, int);
  rt->stop_bits = GET_VAL(dev_conf, "config/stop_bits", int, int);
  rt->uart_idx = GET_VAL(dev_conf, "config/uart_idx", int, int);

  return true;
}
}  // namespace platform::drivers::uart
