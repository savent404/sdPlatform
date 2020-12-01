#include <platform-types.h>

#include <platform/drivers/uart.hxx>

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
  auto rt_ptr = new runtime(tmp_rt);
  if (!rt_ptr) return eno::ENO_NOMEM;
  dev.set_runtime(device::runtime_ptr(rt_ptr));

  // 初始化硬件
  if (!api_.setup) return eno::ENO_NOTIMPL;
  // auto rt = dev.get_runtime().promote<runtime>();
  auto rt = dev.get_runtime().promote<runtime>();
  res = api_.setup(rt);
  return res;
}

int driver::unbind_(device_ref dev) { return eno::ENO_NOPERMIT; }

int driver::open_(device_ref dev, int flags) { return eno::ENO_NOPERMIT; }

int driver::close_(device_ref dev) { return eno::ENO_NOPERMIT; }

int driver::transfer_(device_ref dev, const void* in, size_t in_len, void* out, size_t out_len) {
  return eno::ENO_NOPERMIT;
}

int driver::write_(device_ref dev, const void* out, size_t len) {
  runtime* rt = dev.get_runtime().promote<runtime>();

  if (!api_.tx) return eno::ENO_NOTIMPL;

  return api_.tx(rt, reinterpret_cast<const char*>(out), len);
}

int driver::read_(device_ref dev, void* in, size_t len) { return eno::ENO_NOPERMIT; }

int driver::ioctl_(device_ref dev, int cmds, void* in_out, size_t* in_out_len, size_t max) { return eno::ENO_NOPERMIT; }

cJSON* driver::to_json_() { return nullptr; }
void driver::from_json_(cJSON*) {}
void driver::register_internal_syscall_() {}

#define CHECK_KEY(conf, key) \
  if (!conf.has(key)) return false;
#define GET_VAL(conf, key, internal_type, outside_type) \
  (reinterpret_cast<outside_type>(conf.get<internal_type>(key)))

bool uart::driver::load_device_runtime(parameters_ref dev_conf, runtime* rt) {
  CHECK_KEY(dev_conf, "config/base");
  CHECK_KEY(dev_conf, "config/range");
  CHECK_KEY(dev_conf, "config/irq");
  CHECK_KEY(dev_conf, "config/baudrate");
  CHECK_KEY(dev_conf, "config/parity");
  CHECK_KEY(dev_conf, "config/data_bits");
  CHECK_KEY(dev_conf, "config/stop_bits");
  CHECK_KEY(dev_conf, "config/uart_idx");

  rt->mem_base = GET_VAL(dev_conf, "config/base", int, void*);
  rt->mem_range = GET_VAL(dev_conf, "config/range", int, int);
  rt->irq = GET_VAL(dev_conf, "config/irq", int, int);
  rt->baudrate = GET_VAL(dev_conf, "config/baudrate", int, int);
  rt->parity = GET_VAL(dev_conf, "config/parity", int, int);
  rt->data_bits = GET_VAL(dev_conf, "config/data_bits", int, int);
  rt->stop_bits = GET_VAL(dev_conf, "config/stop_bits", int, int);
  rt->uart_idx = GET_VAL(dev_conf, "config/uart_idx", int, int);

  return true;
}
}  // namespace platform::drivers::uart
