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
#include <requirements.h>

#include <platform/alter/string.hxx>
#include <platform/cJSON.hxx>
#include <platform/driver-dummy.hxx>
#include <platform/entry.hxx>

struct driver_dummy : public platform::driver_dummy {
 public:
  driver_dummy() : platform::driver_dummy() {}
  ~driver_dummy() = default;
 protected:
};

int dummy_driver_id = 0;

extern "C" int dummy_entry(void *env) {
  platform::entry::platform_init(env);

  // register a driver
  auto driver = new driver_dummy;
  auto res = driver->init(0, nullptr);
  if (res != eno::ENO_OK) {
    return res;
  }
  dummy_driver_id = driver->get_id();
  return eno::ENO_OK;
}

driver_entry(dummy_entry, "dummy-device", "dummy");
