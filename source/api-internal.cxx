/**
 * @file api.cxx
 * @author savent (savent_gate@outlook.com)
 * @brief
 * @version 0.1
 * @date 2020-11-17
 *
 * Copyright 2020 jrlc
 *
 */
#include <platform.h>

#ifndef __weak
#define __weak __attribute__((weak))
#endif

#if 1

#include <platform-types.h>

#include <map>
#include <memory>
#include <random>
#include <thread>  //NOLINT

#include <internal/device.hxx>
#include <internal/driver.hxx>

static std::map<int, platform::device *> device_queue;
static std::map<int, platform::driver *> driver_queue;

// __attribute__((constructor)) void devmanager_init(void)
// {

// }

extern "C" {

static bool device_exist(int id) {
  auto iter = device_queue.find(id);
  return iter != device_queue.end();
}
static bool driver_exist(int id) {
  auto iter = driver_queue.find(id);
  return iter != driver_queue.end();
}

__weak int dev_bind(int device_id, int driver_id) {
  if (!driver_exist(driver_id) || !device_exist(device_id)) {
    return eno::ENOTEXIST;
  }

  // TODO(savent): replace it with ipc method
  auto dri_iter = driver_queue.find(driver_id);
  return dri_iter->second->bind(device_id);
}
__weak int dev_unbind(int device_id);

__weak int dev_open(int device_id, int flags);
__weak int dev_close(int device_id);

__weak int dev_transfer(int device_id, const void *in, size_t in_len, void *out, size_t out_len);
__weak int dev_write(int device_id, const void *in, size_t len);
__weak int dev_read(int device_id, void *out, size_t len);

__weak int dev_ioctl(int device_id, void *in_out, size_t *in_out_len, size_t buffer_max_len);

__weak int devmgr_create_device(const char *json_str) {
  auto dev = new platform::device;
  cJSON *obj = cJSON_Parse(json_str);
  dev->from_json(obj);
  cJSON_Delete(obj);

  int id = rand();  // NOLINT
  device_queue[id] = dev;
  return id;
}
__weak int devmgr_update_device(int device_id, const char *json_str) {
  auto dev_iter = device_queue.find(device_id);
  if (dev_iter == device_queue.end()) return eno::ENOTEXIST;
  cJSON *obj = cJSON_Parse(json_str);
  dev_iter->second->from_json(obj);
  cJSON_Delete(obj);
  return 0;
}
__weak int devmgr_remove_device(int device_id) {
  auto dev_iter = device_queue.find(device_id);
  if (dev_iter == device_queue.end()) return 0;
  device_queue.erase(dev_iter);
  return 0;
}
__weak const char *devmgr_query_device(int device_id) {
  auto dev_iter = device_queue.find(device_id);
  if (dev_iter == device_queue.end()) return nullptr;
  return cJSON_Print(device_queue[device_id]->to_json());
}

__weak int devmgr_create_driver(const char *json_str) {
  auto dri = new platform::driver(json_str);
  int id = rand();  // NOLINT
  driver_queue[id] = dri;
  return id;
}

__weak int devmgr_update_driver(int driver_id, const char *json_str) {
  auto dri_iter = driver_queue.find(driver_id);
  if (dri_iter == driver_queue.end()) return eno::ENOTEXIST;
  auto obj = cJSON_Parse(json_str);
  dri_iter->second->from_json(obj);
  cJSON_Delete(obj);
  return 0;
}
__weak int devmgr_remove_driver(int driver_id) {
  auto dri_iter = driver_queue.find(driver_id);
  if (dri_iter == driver_queue.end()) return eno::ENOTEXIST;
  driver_queue.erase(dri_iter);
  return 0;
}
__weak const char *devmgr_query_driver(int driver_id) {
  auto dri_iter = driver_queue.find(driver_id);
  if (dri_iter == driver_queue.end()) return nullptr;
  cJSON* obj = dri_iter->second->to_json();
  auto ret = cJSON_Print(obj);
  cJSON_Delete(obj);
  return ret;
}
}

#else

extern "C" {

__weak int dev_bind(int device_id, int driver_id);
__weak int dev_unbind(int device_id);

__weak int dev_open(int device_id, int flags);
__weak int dev_close(int device_id);

__weak int dev_transfer(int device_id, const void *in, size_t in_len, void *out, size_t out_len);
__weak int dev_write(int device_id, const void *in, size_t len);
__weak int dev_read(int device_id, void *out, size_t len);

__weak int dev_ioctl(int device_id, void *in_out, size_t *in_out_len, size_t buffer_max_len);

__weak int devmgr_create_device(const char *json_str);
__weak int devmgr_update_device(int device_id, const char *json_str);
__weak int devmgr_remove_device(int device_id);
__weak const char *devmgr_query_driver(int driver_id);

__weak int devmgr_create_driver(const char *json_str);
__weak int devmgr_update_driver(int driver_id, const char *json_str);
__weak int devmgr_remove_driver(int driver_id);
__weak const char *devmgr_query_device(int device_id);
}

#endif