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

struct driver_dummy : public platform::driver_dummy {
 public:
  driver_dummy() : platform::driver_dummy() {}
  ~driver_dummy() = default;
};

extern "C" int entry(void) {
  auto driver = new driver_dummy;
  auto res = driver->init(0, nullptr);
  if (res != eno::ENO_OK) { return res; }
}

// static auto _dummyy_xxx __attribute__((section(".initcall0.init"))) = entry;
lvl1_initcall(entry);
