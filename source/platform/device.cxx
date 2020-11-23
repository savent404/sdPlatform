/**
 * @file device.cxx
 * @author savent (savent_gate@outlook.com)
 * @brief
 * @version 0.1
 * @date 2020-11-17
 *
 * Copyright 2020 jrlc
 *
 */
#include <cJSON_Utils.h>

#include <platform.h>
#include <requirements.h>
#include <platform/device.hxx>

namespace platform {

device::device() : id_(0), runtime_(nullptr), config_({}) {}

device::device(parameters_init_list list) : id_(0), runtime_(nullptr), config_(list) {}

cJSON* device::to_json() {
  cJSON* root = cJSON_CreateObject();
  if (id_) {
    cJSON_AddNumberToObject(root, "id", id_);
  }
  if (runtime_) {
    cJSON_AddItemToObject(root, "runtime", runtime_->to_json());
  }
  cJSON_AddItemToObject(root, "config", config_.to_json());
  return root;
}

void device::from_json(cJSON* root) {
  if (cJSON_HasObjectItem(root, "id")) {
    double id = cJSON_GetNumberValue(cJSON_GetObjectItem(root, "id"));
    id_ = static_cast<device_id>(id);
  }
  if (cJSON_HasObjectItem(root, "runtime")) {
    cJSON* item = cJSON_GetObjectItem(root, "runtime");
    if (runtime_) {
      runtime_->from_json(item);
    } else {
      runtime_ = std::make_unique<runtime>();
      runtime_->from_json(item);
    }
  }
  if (cJSON_HasObjectItem(root, "config")) {
    cJSON* item = cJSON_GetObjectItem(root, "config");
    config_.from_json(item);
  }
}

const char* device::to_json_str() {
  cJSON* obj = this->to_json();
  const char* out = cJSON_Print(obj);
  cJSON_Delete(obj);
  return out;
}

void device::from_json_str(const char* ptr) {
  cJSON* obj = cJSON_Parse(ptr);
  this->from_json(obj);
}

device::runtime_ref device::get_runtime() { return *runtime_; }
device::parameters_ref device::get_config() { return config_; }
device::device_id device::get_id() { return id_; }
device::driver_id device::get_bind_id() { return 0; }

device::device_id device::devmgr_register() {
  const char* str = to_json_str();
  int id = devmgr_create_device(str);
  cJSON_free((void*)str);  // NOLINT
  if (id != 0) {
    id_ = id;
    devmgr_update();
  }
  return id_;
}

void device::devmgr_update() {
  const char* str = to_json_str();
  devmgr_update_device(id_, str);
  cJSON_free((void*)str);  // NOLINT
}

void device::devmgr_query() {
  const char* json_str = devmgr_query_device(id_);
  from_json_str(json_str);
  cJSON_free((void*)json_str);  // NOLINT
}

void device::devmgr_delete() { devmgr_remove_device(id_); }

}  // namespace platform
