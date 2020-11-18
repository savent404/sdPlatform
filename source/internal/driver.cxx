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
#include <internal/driver.hxx>

namespace platform {

driver::driver(parameters::initial_list config_list, runtime_ptr runtime)
    : id_(0), config_(config_list), runtime_p_(std::move(runtime)), device_list_() {}

driver::driver(const char *json) : id_(0), config_({}), runtime_p_(), device_list_() {
  cJSON *obj = cJSON_Parse(json);
  from_json(obj);
  cJSON_Delete(obj);
}

driver::~driver() {}

int driver::init(int argc, char **argv) { return init_(argc, argv); }

int driver::deinit() { return deinit_(); }

static bool match_compat(const char *v1, const char *v2) {
  std::string s1(v1);
  std::string s2(v2);
  auto pos = s1.find('|');
  do {
    std::string sub = s1.substr(0, pos);
    if (s2.find(sub)) return true;
    s1 = s1.substr(pos + 1);
  } while (pos != std::string::npos);

  return false;
}

int driver::bind(device_id dev) {
  auto ptr = query_device(dev);
  if (!ptr) return eno::ENOTEXIST;

  // check compat string
  if (!get_config().has("compat") || !ptr->get_config().has("compat")) return eno::EINVALID;
  const char *driver_compat(std::get<const char *>(get_config().get("compat")));
  const char *device_compat(std::get<const char *>(ptr->get_config().get("compat")));
  if (!match_compat(driver_compat, device_compat)) return eno::EINVALID;

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

cJSON *driver::to_json() {
  cJSON *config = get_config().to_json();
  cJSON *subdev = to_json_();

  cJSON *root = cJSON_CreateObject();
  cJSON_AddObjectToObject(config, "config");
  cJSON_AddObjectToObject(subdev, "subdev");
  if (id_) {
    cJSON_AddNumberToObject(root, "id", id_);
  }
  return root;
}

void driver::from_json(cJSON *obj) {
  if (cJSON_HasObjectItem(obj, "config")) {
    get_config().from_json(cJSON_GetObjectItem(obj, "config"));
  }
  if (cJSON_HasObjectItem(obj, "subdev")) {
    from_json_(cJSON_GetObjectItem(obj, "subdev"));
  }
  if (cJSON_HasObjectItem(obj, "id")) {
    int id = cJSON_GetNumberValue(cJSON_GetObjectItem(obj, "id"));
    id_ = id;
  }
}

driver::driver_id driver::get_id() { return id_; }

driver::driver_id driver::devmgr_register() {
  cJSON *root = cJSON_CreateObject();
  if (id_) {
    cJSON_AddNumberToObject(root, "id", get_id());
  }
  cJSON_AddItemToObject(root, "config", get_config().to_json());
  cJSON_AddItemToObject(root, "subdev", to_json_());

  const char *str = cJSON_Print(root);
  driver_id id = devmgr_create_driver(str);
  cJSON_Delete(root);
  cJSON_free((void *)str);  // NOLINT
  if (id) {
    id_ = id;
    devmgr_update();
  }
  return id_;
}

void driver::devmgr_update() {
  cJSON *root = to_json();
  const char *str = cJSON_Print(root);
  devmgr_update_driver(id_, str);
  cJSON_Delete(root);
  cJSON_free((void *)str);  // NOLINT
}

void driver::devmgr_query() {
  const char *str = devmgr_query_driver(id_);
  cJSON *obj = cJSON_Parse(str);
  from_json(obj);
  cJSON_Delete(obj);
}

void driver::devmgr_delete() { devmgr_remove_driver(id_); }

driver::parameters_ref driver::get_config() { return config_; }

driver::runtime_ref driver::get_runtime() { return *runtime_p_; }

driver::device_ptr driver::query_device(device_id id) {
  auto dev = std::make_unique<device>();
  const char *json_str = devmgr_query_device(id);
  cJSON *json = cJSON_Parse(json_str);
  dev->from_json(json);

  return std::move(dev);
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
