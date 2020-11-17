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
#include <internal/driver.hxx>

namespace platform {

driver::driver(parameters::initial_list config_list, runtime_ptr runtime)
    : config_(config_list), runtime_p_(std::move(runtime)), device_list_() {}

int driver::init(int argc, char **argv) { return init_(argc, argv); }

int driver::deinit() { return deinit_(); }

int driver::bind(device_id dev) {
  auto ptr = query_device(dev);
  if (!ptr) return eno::ENOTEXIST;
  auto res = bind_(*ptr);
  if (eno::EOK == res) {
    add_device(dev, std::move(ptr));
  }
  return res;
}

int driver::unbind(device_id dev) {
  auto ptr = get_device(dev);
  if (!ptr) return eno::ENOTEXIST;
  auto res = unbind_(*ptr);
  put_device(dev, std::move(ptr));
  return res;
}

int driver::open(device_id dev, int flags) {
  auto ptr = get_device(dev);
  if (!ptr) return eno::ENOTEXIST;
  auto res = open_(*ptr, flags);
  put_device(dev, std::move(ptr));
  return res;
}

int driver::close(device_id dev) {
  auto ptr = get_device(dev);
  if (!ptr) return eno::ENOTEXIST;
  auto res = close_(*ptr);
  put_device(dev, std::move(ptr));
  return res;
}

int driver::transfer(device_id dev, const void *in, size_t in_len, void *out, size_t out_len) {
  auto ptr = get_device(dev);
  if (!ptr) return eno::ENOTEXIST;
  auto res = transfer_(*ptr, in, in_len, out, out_len);
  put_device(dev, std::move(ptr));
  return res;
}

int driver::write(device_id dev, const void *in, size_t len) {
  auto ptr = get_device(dev);
  if (!ptr) return eno::ENOTEXIST;
  auto res = write_(*ptr, in, len);
  put_device(dev, std::move(ptr));
  return res;
}

int driver::read(device_id dev, void *out, size_t len) {
  auto ptr = get_device(dev);
  if (!ptr) return eno::ENOTEXIST;
  auto res = read_(*ptr, out, len);
  put_device(dev, std::move(ptr));
  return res;
}

int driver::ioctl(device_id dev, int cmds, void *in_out, size_t *in_out_len, size_t max_len) {
  auto ptr = get_device(dev);
  if (!ptr) return eno::ENOTEXIST;
  auto res = ioctl_(*ptr, cmds, in_out, in_out_len, max_len);
  put_device(dev, std::move(ptr));
  return res;
}

cJSON *driver::toJson() {
  cJSON *config = get_config().toJson();
  cJSON *runtime = get_config().toJson();
  cJSON *subdev = toJson_();

  cJSON *root = cJSON_CreateObject();
  cJSON_AddObjectToObject(config, "config");
  cJSON_AddObjectToObject(runtime, "runtime");
  cJSON_AddObjectToObject(subdev, "subdev");

  return root;
}

driver::parameters_ref driver::get_config() { return config_; }

driver::runtime_ref driver::get_runtime() { return *runtime_p_; }

driver::device_ptr driver::query_device(device_id id) {
  // TODO(savent): need implement
  return std::make_unique<device>();
}

bool driver::update_device(device_id id, device_ref dev) { return true; }

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

bool driver::has_device(device_id id) { return device_list_iter_handle(device_list_, id) == device_list_.end(); }

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

}  // namespace platform
