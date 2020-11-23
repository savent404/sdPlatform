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

#include <platform/alter/map.hxx>
#include <platform/device.hxx>
#include <platform/driver.hxx>

#ifndef __weak
#define __weak __attribute__((weak))
#endif

static auto &get_driver_map() {
  static platform::alter::map<int, platform::driver::driver_ptr> *driver_map;
  if (!driver_map) driver_map = new platform::alter::map<int, platform::driver::driver_ptr>;
  return *driver_map;
}

static auto &get_device_map() {
  static platform::alter::map<int, platform::device::device_ptr> *device_map;
  if (!device_map) device_map = new platform::alter::map<int, platform::device::device_ptr>;
  return *device_map;
}

static bool driver_exist(int id) {
  auto iter = get_driver_map().find(id);
  return get_driver_map().end() != iter;
}

static bool device_exist(int id) {
  auto iter = get_device_map().find(id);
  return get_device_map().end() != iter ? !(iter->second) : false;
}

static int get_driver_id(int id) {
  auto iter = get_device_map().find(id);
  if (iter == get_device_map().end() || !(iter->second)) return 0;
  return iter->second->get_bind_id();
};

extern "C" {

int dev_bind(int device_id, int driver_id) {
  auto iter = get_driver_map().find(driver_id);
  if (iter == get_driver_map().end() || !(iter->second)) return eno::ENO_NOTEXIST;
  return iter->second->bind(device_id);
  // TODO(savent): need add device info to map
}
int dev_unbind(int device_id) {
  int driver_id = get_driver_id(device_id);
  if (!driver_id) return eno::ENO_NOTEXIST;
  auto iter = get_driver_map().find(driver_id);
  if (iter == get_driver_map().end() || !(iter->second)) return eno::ENO_NOTEXIST;
  return iter->second->unbind(device_id);
  // TODO(savent): need update device info
}

int dev_open(int device_id, int flags) {
  int driver_id = get_driver_id(device_id);
  if (!driver_id) return eno::ENO_NOTEXIST;
  auto iter = get_driver_map().find(driver_id);
  if (iter == get_driver_map().end() || !(iter->second)) return eno::ENO_NOTEXIST;
  return iter->second->open(device_id, flags);
}

int dev_close(int device_id) {
  int driver_id = get_driver_id(device_id);
  if (!driver_id) return eno::ENO_NOTEXIST;
  auto iter = get_driver_map().find(driver_id);
  if (iter == get_driver_map().end() || !(iter->second)) return eno::ENO_NOTEXIST;
  return iter->second->close(device_id);
}

int dev_transfer(int device_id, const void *in, size_t in_len, void *out, size_t out_len)
{
  int driver_id = get_driver_id(device_id);
  if (!driver_id) return eno::ENO_NOTEXIST;
  auto iter = get_driver_map().find(driver_id);
  if (iter == get_driver_map().end() || !(iter->second)) return eno::ENO_NOTEXIST;
  return iter->second->transfer(device_id, in, in_len, out, out_len);
}
int dev_write(int device_id, const void *in, size_t len)
{
  int driver_id = get_driver_id(device_id);
  if (!driver_id) return eno::ENO_NOTEXIST;
  auto iter = get_driver_map().find(driver_id);
  if (iter == get_driver_map().end() || !(iter->second)) return eno::ENO_NOTEXIST;
  return iter->second->write(device_id, in, len);
}
int dev_read(int device_id, void *out, size_t len) {
  int driver_id = get_driver_id(device_id);
  if (!driver_id) return eno::ENO_NOTEXIST;
  auto iter = get_driver_map().find(driver_id);
  if (iter == get_driver_map().end() || !(iter->second)) return eno::ENO_NOTEXIST;
  return iter->second->read(device_id, out, len);
}

int dev_ioctl(int device_id, int cmds, void *in_out, size_t *in_out_len, size_t buffer_max_len) {
  int driver_id = get_driver_id(device_id);
  if (!driver_id) return eno::ENO_NOTEXIST;
  auto iter = get_driver_map().find(driver_id);
  if (iter == get_driver_map().end() || !(iter->second)) return eno::ENO_NOTEXIST;
  return iter->second->ioctl(device_id, cmds, in_out, in_out_len, buffer_max_len);
}

}
