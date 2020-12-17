/**
 * @file requirements.cxx
 * @author savent (savent_gate@outlook.com)
 * @brief 对于requirements.h中需要函数的空实现，用于内部测试
 * @note 函数已标及为weak,实际链接时使用外部提供的符号
 * @version 0.1
 * @date 2020-11-23
 *
 * Copyright 2020 jrlc
 *
 */

#include <platform.h>

#include <platform-types.h>

#include <random>

#include <platform/alter/map.hxx>
#include <platform/device.hxx>
#include <platform/driver-dummy.hxx>
#include <platform/driver.hxx>
#include <platform/entry.hxx>

#ifndef __weak
#define __weak __attribute__((weak))
#endif

std::map<int, platform::device *> &device_queue() {
  static std::map<int, platform::device *> *instance;
  if (!instance) instance = new std::map<int, platform::device *>;
  return *instance;
}

std::map<int, platform::driver_dummy *> &driver_queue() {
  static std::map<int, platform::driver_dummy *> *instance;
  if (!instance) instance = new std::map<int, platform::driver_dummy *>;
  return *instance;
}

extern "C" {

__weak int devmgr_create_device(const char *json_str, int i) {
  auto dev = new platform::device;
  dev->from_json_str(json_str);
  int id = rand();  // NOLINT
  device_queue()[id] = dev;
  return id;
}
__weak int devmgr_update_device(int device_id, const char *json_str) {
  auto dev_iter = device_queue().find(device_id);
  if (dev_iter == device_queue().end()) return eno::ENO_NOTEXIST;
  dev_iter->second->from_json_str(json_str);
  return 0;
}
__weak int devmgr_remove_device(int device_id) {
  auto dev_iter = device_queue().find(device_id);
  if (dev_iter == device_queue().end()) return 0;
  device_queue().erase(dev_iter);
  return 0;
}
__weak const char *devmgr_query_device(int device_id) {
  auto dev_iter = device_queue().find(device_id);
  if (dev_iter == device_queue().end()) return nullptr;
  return device_queue()[device_id]->to_json_str();
}

__weak int devmgr_create_driver(const char *json_str) {
  auto dir = new platform::driver_dummy;
  dir->from_json_str(json_str);
  int id = rand();  // NOLINT
  driver_queue()[id] = dir;
  return id;
}

__weak int devmgr_update_driver(int driver_id, const char *json_str) {
  auto dri_iter = driver_queue().find(driver_id);
  if (dri_iter == driver_queue().end()) return eno::ENO_NOTEXIST;
  dri_iter->second->from_json_str(json_str);
  return 0;
}
__weak int devmgr_remove_driver(int driver_id) {
  auto dri_iter = driver_queue().find(driver_id);
  if (dri_iter == driver_queue().end()) return eno::ENO_NOTEXIST;
  driver_queue().erase(dri_iter);
  return 0;
}
__weak const char *devmgr_query_driver(int driver_id) {
  auto dri_iter = driver_queue().find(driver_id);
  if (dri_iter == driver_queue().end()) return nullptr;
  return dri_iter->second->to_json_str();
}


extern driver_init_t __driver_init_start;
extern driver_init_t __driver_init_end;

static int devmgr_entry() {
  auto ptr = &__driver_init_start;
  auto end = &__driver_init_end;
  while (ptr != end) {
    ptr->entry(nullptr);
    ptr++;
  }
  return 0;
}

func_entry_level_high(devmgr_entry);
}
