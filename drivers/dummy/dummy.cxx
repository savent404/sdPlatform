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
#include <platform/driver-dummy.hxx>
#include <platform/driver.hxx>
#include <functional>
#include <initcall.h>

struct driver_dummy : public platform::driver_dummy {
 public:
  driver_dummy() : platform::driver_dummy() {}
  ~driver_dummy() = default;
};



extern "C"
int entry(void) {
    auto driver = new driver_dummy;
    return driver->init(0, nullptr);
}

// static auto _dummyy_xxx __attribute__((section(".initcall0.init"))) = entry;
lvl1_initcall(entry);
