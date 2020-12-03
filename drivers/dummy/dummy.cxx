/**
 * @file dummy.cxx
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
#include <platform/cJSON.hxx>
#include <platform/driver-dummy.hxx>
#include <platform/entry.hxx>

#include <requirements.h>

#include <platform/alter/string.hxx>
#include <platform/entry.hxx>

struct driver_dummy : public platform::driver_dummy {
 public:
  driver_dummy() : platform::driver_dummy() {}
  ~driver_dummy() = default;
};

extern "C" int dummy_entry(void) {
  platform::entry::platform_init();

  // register a driver
  auto driver = new driver_dummy;
  auto res = driver->init(0, nullptr);
  if (res != eno::ENO_OK) {
    return res;
  }
  auto driver_id = driver->get_id();

  // register a device
  auto device = new platform::device({{"name", "dummy-device"}, {"compat", "dummy"}});
  auto str = device->to_json_str();
  auto device_id = devmgr_create_device(str, device->get_id());
  device->set_id(device_id);
  platform::cJSON_free((char *)(str));
  str = device->to_json_str();
  devmgr_update_device(device_id, str);
  platform::cJSON_free((char *)str);

  return dev_bind(device_id, driver_id);
}

func_entry_level_default(dummy_entry);
