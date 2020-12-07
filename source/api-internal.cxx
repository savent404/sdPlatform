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
#include <platform-types.h>
#include <platform.h>
#include <requirements.h>

#include <map>
#include <memory>
#include <random>

// clang-format off
#include <platform/alter/map.hxx>
#include <platform/device.hxx>
#include <platform/driver-dummy.hxx>
#include <platform/driver.hxx>
// clang-format on

#ifndef __weak
#define __weak __attribute__((weak))
#endif

static auto &get_driver_map() {
  static platform::alter::map<int, platform::driver::driver_ptr> *driver_map;
  if (!driver_map) driver_map = new platform::alter::map<int, platform::driver::driver_ptr>;
  return *driver_map;
}

extern "C" void heap_free(void *);
static void free_devmgrs_string(const char *str) { heap_free(const_cast<char *>(str)); }

static auto &get_device_map() {
  static platform::alter::map<int, platform::device::device_ptr> *device_map;
  if (!device_map) device_map = new platform::alter::map<int, platform::device::device_ptr>;
  return *device_map;
}

static int search_driver(int driver_id, std::map<int, platform::driver::driver_ptr>::iterator *item_iter) {
  auto iter = get_driver_map().find(driver_id);
  if (iter == get_driver_map().end() || !(iter->second)) {
    auto drv_str = devmgr_query_driver(driver_id);
    if (!drv_str) {
      return eno::ENO_NOTEXIST;
    }
    auto drv_ptr = new platform::driver_dummy;
    if (!drv_ptr) {
      return eno::ENO_NOMEM;
    }
    drv_ptr->from_json_str(drv_str);
    free_devmgrs_string(drv_str);
    get_driver_map()[driver_id] = platform::driver::driver_ptr(drv_ptr);
    iter = get_driver_map().find(driver_id);
  }
  *item_iter = iter;
  return eno::ENO_OK;
}

static int search_device(int device_id, std::map<int, platform::device::device_ptr>::iterator *item_iter) {
  auto iter = get_device_map().find(device_id);
  if (iter == get_device_map().end() || !(iter->second)) {
    // need query device manager
    auto dev_str = devmgr_query_driver(device_id);
    if (!dev_str) {
      return eno::ENO_NOTEXIST;
    }
    auto dev_ptr = new platform::device();
    if (!dev_ptr) {
      return eno::ENO_NOMEM;
    }
    dev_ptr->from_json_str(dev_str);
    free_devmgrs_string(dev_str);
    get_device_map()[device_id] = platform::device::device_ptr(dev_ptr);
    iter = get_device_map().find(device_id);
  }
  *item_iter = iter;
  return eno::ENO_OK;
}

static int update_device_info(int device_id) {
  const char *dev_str = devmgr_query_device(device_id);
  if (!dev_str) return eno::ENO_NOTEXIST;
  auto iter = get_device_map().end();
  int res = search_device(device_id, &iter);
  if (res) return res;
  iter->second->from_json_str(dev_str);
  return eno::ENO_OK;
}

static int get_driver_id(int device_id) {
  auto iter = get_device_map().end();
  int res = search_device(device_id, &iter);
  if (res) {
    return res;
  }
  return iter->second->get_bind_id();
}

bool platform::_register_driver_hook_(platform::driver *ptr) {
  if (ptr && ptr->get_id()) {
    get_driver_map()[ptr->get_id()] = platform::driver::driver_ptr(ptr);
  }
  return true;
}

extern "C" {

int dev_bind(int device_id, int driver_id) {
  auto iter = get_driver_map().end();
  int res = search_driver(driver_id, &iter);
  if (res) return res;

  // TODO(savent): call this using ipc method
  return iter->second->bind(device_id);
}

int dev_unbind(int device_id) {
  int driver_id = get_driver_id(device_id);
  if (!driver_id) return eno::ENO_NOTEXIST;
  auto iter = get_driver_map().end();
  int res = search_driver(driver_id, &iter);
  if (res) return res;
  update_device_info(device_id);
  return iter->second->unbind(device_id);
}

int dev_open(int device_id, int flags) {
  int driver_id = get_driver_id(device_id);
  auto iter = get_driver_map().end();
  int res = search_driver(driver_id, &iter);
  if (res) return res;
  return iter->second->open(device_id, flags);
}

int dev_close(int device_id) {
  int driver_id = get_driver_id(device_id);
  auto iter = get_driver_map().end();
  int res = search_driver(driver_id, &iter);
  if (res) return res;
  return iter->second->close(device_id);
}

int dev_transfer(int device_id, const void *in, size_t in_len, void *out, size_t out_len) {
  int driver_id = get_driver_id(device_id);
  auto iter = get_driver_map().end();
  int res = search_driver(driver_id, &iter);
  if (res) return res;
  return iter->second->transfer(device_id, in, in_len, out, out_len);
}
int dev_write(int device_id, const void *in, size_t len) {
  int driver_id = get_driver_id(device_id);
  auto iter = get_driver_map().end();
  int res = search_driver(driver_id, &iter);
  if (res) return res;
  return iter->second->write(device_id, in, len);
}
int dev_read(int device_id, void *out, size_t len) {
  int driver_id = get_driver_id(device_id);
  auto iter = get_driver_map().end();
  int res = search_driver(driver_id, &iter);
  if (res) return res;
  return iter->second->read(device_id, out, len);
}

int dev_ioctl(int device_id, int cmds, void *in_out, size_t *in_out_len, size_t buffer_max_len) {
  int driver_id = get_driver_id(device_id);
  auto iter = get_driver_map().end();
  int res = search_driver(driver_id, &iter);
  if (res) return res;
  return iter->second->ioctl(device_id, cmds, in_out, in_out_len, buffer_max_len);
}
}
