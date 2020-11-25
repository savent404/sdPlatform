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

#include <initcall.h>
#include <platform-types.h>
#include <platform.h>
#include <functional>
#include <platform/driver-dummy.hxx>
#include <platform/driver.hxx>
#include <platform/cJSON.hxx>

#include <requirements.h>

#include <smempool.h>

struct driver_dummy : public platform::driver_dummy {
 public:
  driver_dummy() : platform::driver_dummy() {
  }
  ~driver_dummy() = default;
};

extern "C" void init_mempool();
extern "C" int entry(void) {
  void * ptr = nullptr;
  init_mempool();
  platform::cJSON_Hooks hooks = {
    .malloc_fn = smem_alloc,
    .free_fn = smem_free,
  };
  platform::cJSON_InitHooks(&hooks);

  // register a driver
  auto driver = new driver_dummy;
  auto res = driver->init(0, nullptr);
  if (res != eno::ENO_OK) { return res; }
  auto driver_id = driver->get_id();

  // register a device
  auto device = new platform::device({
    {"name", "dummy-device"},
    {"compat", "dummy"}
  });
  auto str = device->to_json_str();
  auto device_id = devmgr_create_device(str);
  device->set_id(device_id);
  platform::cJSON_free((char *)(str));
  str = device->to_json_str();
  devmgr_update_device(device_id, str);
  platform::cJSON_free((char*)str);


  return dev_bind(device_id, driver_id);
}

lvl0_initcall(entry);
