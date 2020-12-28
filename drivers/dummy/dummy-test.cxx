/**
 * @file dummy-test.cxx
 * @author savent (savent_gate@outlook.com)
 * @brief
 * @version 0.1
 * @date 2020-11-23
 *
 * Copyright 2020 jrlc
 *
 */

#include <platform-types.h>
#include <platform.h>
#include <requirements.h>

#include <platform/alter/string.hxx>
#include <platform/cJSON.hxx>
#include <platform/debug.hxx>
#include <platform/driver-dummy.hxx>
#include <platform/entry.hxx>
#include <platform/os.hxx>

#include "../../../include/platform.h"

using debug = platform::debug;

extern int dummy_driver_id;

extern "C" void* dummy_device_bind_test_it(void* p) {
  while (dummy_driver_id == 0) {
    platform::ops::thread::thread_sleep(100);
  }
  // register a device
  auto device = new platform::device({{"name", "dummy-device"}, {"compat", "dummy"}});
  auto str = device->to_json_str();
  auto device_id = devmgr_create_device(str);
  device->set_id(device_id);
  platform::cJSON_free((char*)(str));  // NOLINT
  str = device->to_json_str();
  devmgr_update_device(device_id, str);
  platform::cJSON_free((char*)str);  // NOLINT

  dev_bind(device_id, dummy_driver_id);
  debug::assert(dev_open(device_id, fflag::FF_READ | fflag::FF_WRITE) == eno::ENO_OK);
  char recv[6];
  debug::assert(dev_read(device_id, recv, sizeof(recv)) == eno::ENO_OK);
  debug::assert(!strcmp("world", recv));
  debug::assert(dev_write(device_id, "Hello", 6) == eno::ENO_OK);
  memset(recv, 0, sizeof(recv));
  debug::assert(dev_transfer(device_id, "Hello", 6, recv, sizeof(recv)) == eno::ENO_OK);
  char msg[] = "Hello";
  debug::assert(dev_ioctl(device_id, 0, msg, 6, 6) == eno::ENO_OK);
  debug::assert(!strcmp(msg, "world"));
  debug::assert(dev_close(device_id) == eno::ENO_OK);
  debug::assert(dev_unbind(device_id) == eno::ENO_OK);
  platform::ops::thread::thread_exit(0);
  return NULL;
}
extern "C" int dummy_device_bind_test(void) {
  platform::ops::thread::thread_create(dummy_device_bind_test_it, nullptr);
  return 0;
}

// func_entry_level_untest(dummy_device_bind_test);
