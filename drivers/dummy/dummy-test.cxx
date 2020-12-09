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
#include <osal.h>

#include <platform/alter/string.hxx>
#include <platform/cJSON.hxx>
#include <platform/driver-dummy.hxx>
#include <platform/entry.hxx>

struct driver_dummy : public platform::driver_dummy {
 public:
  driver_dummy() : platform::driver_dummy() {}
  ~driver_dummy() = default;
};

extern int dummy_driver_id;

extern "C" void* dummy_device_bind_test_it(void* p) {
  while (dummy_driver_id == 0) {
    os.os_msleep(100);
  }
  // register a device
  auto device = new platform::device({{"name", "dummy-device"}, {"compat", "dummy"}});
  auto str = device->to_json_str();
  auto device_id = devmgr_create_device(str);
  device->set_id(device_id);
  platform::cJSON_free((char *)(str)); // NOLINT
  str = device->to_json_str();
  devmgr_update_device(device_id, str);
  platform::cJSON_free((char *)str);  // NOLINT
  
  dev_bind(device_id, dummy_driver_id);
  os.os_exit(0);
  return NULL;
}
extern "C" int dummy_device_bind_test(void) {
  os.os_thread_create(dummy_device_bind_test_it, nullptr);
  return 0;
}

func_entry_level_untest(dummy_device_bind_test);

