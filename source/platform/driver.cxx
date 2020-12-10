/**
 * @file driver.cxx
 * @author savent (savent_gate@outlook.com)
 * @brief
 * @version 0.1
 * @date 2020-11-17
 *
 * Copyright 2020 jrlc
 *
 */
#include <platform-types.h>
#include <platform.h>
#include <requirements.h>

#include <platform/alter/string.hxx>
#include <platform/debug.hxx>
#include <platform/driver.hxx>
#include <platform/entry.hxx>
#include <platform/syscall.hxx>

namespace platform {

using string = alter::string;

driver::driver(parameters::initial_list config_list, runtime_ptr runtime)
    : config_(config_list), runtime_(std::move(runtime)), device_list_() {}

driver::driver(const char *json) : config_({}), runtime_(), device_list_() { from_json_str(json); }

driver::~driver() {}

int driver::init(int argc, char **argv) {
  auto id = devmgr_register();
  if (!id) {
    // TODO(savent): warning if register no registed
  }
  // _register_driver_hook_(this);
  return init_(argc, argv);
}

int driver::deinit() { return deinit_(); }

static bool match_compat(const char *v1, const char *v2) {
  string s1(v1);
  string s2(v2);
  auto pos = s1.find('|');
  do {
    string sub = s1.substr(0, pos);
    if (s2.find(sub) != string::npos) return true;
    s1 = s1.substr(pos + 1);
  } while (pos != string::npos);

  return false;
}

int driver::bind(device_id dev) {
  auto ptr = query_device(dev);
  if (!ptr) return eno::ENO_NOTEXIST;

  // check compat string
  if (!get_config().has("compat") || !ptr->get_config().has("compat")) return eno::ENO_INVALID;
  const char *driver_compat(std::get<const char *>(get_config().get("compat")));
  const char *device_compat(std::get<const char *>(ptr->get_config().get("compat")));
  if (!match_compat(driver_compat, device_compat)) return eno::ENO_INVALID;

  auto res = bind_(*ptr);
  if (eno::ENO_OK == res) {
    ptr->set_bind_id(get_id());
    update_device(dev, *ptr);
    add_device(dev, std::move(ptr));
  }
  return res;
}

int driver::unbind(device_id dev) {
  auto ptr = get_device(dev);
  if (!ptr) return eno::ENO_NOTEXIST;
  auto res = unbind_(*ptr);
  put_device(dev, std::move(ptr));
  return res;
}

int driver::open(device_id dev, int flags) {
  auto ptr = get_device(dev);
  if (!ptr) return eno::ENO_NOTEXIST;
  auto res = open_(*ptr, flags);
  put_device(dev, std::move(ptr));
  return res;
}

int driver::close(device_id dev) {
  auto ptr = get_device(dev);
  if (!ptr) return eno::ENO_NOTEXIST;
  auto res = close_(*ptr);
  put_device(dev, std::move(ptr));
  return res;
}

int driver::transfer(device_id dev, const void *in, size_t in_len, msg *msgbuf) {
  auto ptr = get_device(dev);
  if (!ptr) return eno::ENO_NOTEXIST;
  auto res = transfer_(*ptr, in, in_len, msgbuf->get(), msgbuf->size());
  put_device(dev, std::move(ptr));
  return res;
}

int driver::write(device_id dev, const void *in, size_t len) {
  auto ptr = get_device(dev);
  if (!ptr) return eno::ENO_NOTEXIST;
  auto res = write_(*ptr, in, len);
  put_device(dev, std::move(ptr));
  return res;
}

int driver::read(device_id dev, msg *msgbuf) {
  auto ptr = get_device(dev);
  if (!ptr) return eno::ENO_NOTEXIST;
  auto res = read_(*ptr, msgbuf->get(), msgbuf->size());
  put_device(dev, std::move(ptr));
  return res;
}

int driver::ioctl(device_id dev, int cmds, const void *in, size_t in_len, msg *msgbuf) {
  auto ptr = get_device(dev);
  if (!ptr) return eno::ENO_NOTEXIST;
  size_t sz = msgbuf->size();
  auto res = ioctl_(*ptr, cmds, in, in_len, msgbuf->get(), &sz, msgbuf->size());
  if (!msgbuf->resize(sz, false)) return eno::ENO_NOMEM;
  put_device(dev, std::move(ptr));
  return res;
}

cJSON *driver::to_json() {
  cJSON *root = config_.to_json();
  if (runtime_) {
    cJSON_AddItemToObject(root, "runtime", runtime_->to_json());
  }
  auto ipc_obj = entry::get_ipc_description();
  auto ipc_json = ipc_obj.to_json();
  cJSON_AddItemToObject(root, "ipc", ipc_json);
  to_json_(root);
  return root;
}

void driver::from_json(cJSON *obj) {
  config_.from_json(obj);
  if (cJSON_HasObjectItem(obj, "runtime")) {
    cJSON *item = cJSON_GetObjectItem(obj, "runtime");
    if (runtime_) {
      runtime_->from_json(item);
    }
  }
  from_json_(obj);
  // NOTE(savent): attribute 'ipc' only used in driver-dummy
}

const char *driver::to_json_str() {
  cJSON *obj = to_json();
  const char *out = cJSON_PrintUnformatted(obj);
  cJSON_Delete(obj);
  return out;
}

void driver::from_json_str(const char *json) {
  cJSON *obj = cJSON_Parse(json);
  from_json(obj);
  cJSON_Delete(obj);
}

const char *driver::get_name() {
  if (get_config().has("name")) {
    return get_config().get<const char *>("name");
  }
  return nullptr;
}

const char *driver::get_compat() {
  if (get_config().has("compat")) {
    return get_config().get<const char *>("compat");
  }
  return 0;
}

driver::driver_id driver::get_id() {
  if (get_config().has("id")) {
    return get_config().get<int>("id");
  }
  return 0;
}
void driver::set_id(driver_id id) { get_config().set("id", id); }

driver::driver_id driver::devmgr_register() {
  const char *str = to_json_str();
  if (!str) return 0;
  driver_id id = devmgr_create_driver(str);
  cJSON_free((void *)str);  // NOLINT
  if (id) {
    set_id(id);
    devmgr_update();
    register_syscall();
  }
  return id;
}

void driver::devmgr_update() {
  debug::assert(get_id());
  const char *str = to_json_str();
  devmgr_update_driver(get_id(), str);
  cJSON_free((void *)str);  // NOLINT
}

void driver::devmgr_query() {
  debug::assert(get_id());
  const char *str = devmgr_query_driver(get_id());
  from_json_str(str);
  cJSON_free((void *)str);  // NOLINT
}

void driver::devmgr_delete() {
  debug::assert(get_id());
  devmgr_remove_driver(get_id());
}

driver::parameters_ref driver::get_config() { return config_; }

driver::runtime_ref driver::get_runtime() { return *runtime_; }

driver::device_ptr driver::query_device(device_id id) {
  auto dev = new device;
  const char *json_str = devmgr_query_device(id);
  dev->from_json_str(json_str);
  cJSON_free((void *)json_str);  // NOLINT
  return std::unique_ptr<device>(dev);
}

bool driver::update_device(device_id id, device_ref dev) {
  dev.set_id(id);
  const char *str = dev.to_json_str();
  auto res = devmgr_update_device(id, str);
  platform::cJSON_free((void *)str);  // NOLINT
  return res == 0;
}

static auto device_list_iter_handle = [](driver::device_kv_list &list, driver::driver_id id) {
  auto iter = list.begin();
  for (; iter != list.end(); ++iter) {
    if (iter->first == id) return iter;
  }
  return iter;
};

bool driver::add_device(device_id id, device_ptr ptr) {
  if (has_device(id)) return false;
  device_list_.push_back(std::make_pair(id, std::move(ptr)));
  return true;
}

bool driver::has_device(device_id id) { return device_list_iter_handle(device_list_, id) != device_list_.end(); }

bool driver::del_device(device_id id) {
  auto iter = device_list_iter_handle(device_list_, id);
  if (iter == device_list_.end()) return false;
  device_list_.erase(iter);
  return true;
}

driver::device_ptr driver::get_device(device_id id) {
  auto iter = device_list_iter_handle(device_list_, id);
  if (iter == device_list_.end()) return nullptr;
  return std::move(iter->second);
}

bool driver::put_device(device_id id, device_ptr ptr) {
  auto iter = device_list_iter_handle(device_list_, id);
  if (iter == device_list_.end()) return false;
  iter->second = std::move(ptr);
  return true;
}

void driver::register_syscall() {
  using namespace std::placeholders;  // NOLINT
  string prefix;
  if (!get_config().has("name")) {
    prefix = "unknow-name-";
  } else {
    prefix = std::get<const char *>(get_config().get("name"));
    prefix += "-";
  }

  auto syscall_handler = syscall::get_instance();

  syscall_handler->add(prefix + "deinit", static_cast<std::function<int(void)>>(std::bind(&driver::deinit, this)));
  syscall_handler->add(prefix + "bind", static_cast<std::function<int(int)>>(std::bind(&driver::bind, this, _1)));
  syscall_handler->add(prefix + "unbind", static_cast<std::function<int(int)>>(std::bind(&driver::unbind, this, _1)));
  syscall_handler->add(prefix + "open",
                       static_cast<std::function<int(int, int)>>(std::bind(&driver::open, this, _1, _2)));
  syscall_handler->add(prefix + "close", static_cast<std::function<int(int)>>(std::bind(&driver::close, this, _1)));
  syscall_handler->add(prefix + "transfer", static_cast<std::function<int(int, void *, size_t, msg *)>>(
                                                std::bind(&driver::transfer, this, _1, _2, _3, _4)));
  syscall_handler->add(prefix + "write", static_cast<std::function<int(int, void *, size_t)>>(
                                             std::bind(&driver::write, this, _1, _2, _3)));
  syscall_handler->add(prefix + "read",
                       static_cast<std::function<int(int, msg *)>>(std::bind(&driver::read, this, _1, _2)));
  syscall_handler->add(prefix + "ioctl",
                       static_cast<std::function<int(int, int, void *, size_t, msg*)>>(
                           std::bind(&driver::ioctl, this, _1, _2, _3, _4, _5)));
  register_internal_syscall_();
}

}  // namespace platform
