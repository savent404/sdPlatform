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

struct driver_dummy : public platform::driver_dummy {
 public:
  driver_dummy() : platform::driver_dummy() {
  }
  ~driver_dummy() = default;
};

extern "C" void* heap_alloc(size_t n);
extern "C" void heap_free(void*);

extern "C" int entry(void) {
  void * ptr = nullptr;

  platform::cJSON_Hooks hooks = {
    .malloc_fn = heap_alloc,
    .free_fn = heap_free,
  };
  platform::cJSON_InitHooks(&hooks);

  auto driver = new driver_dummy;
  auto res = driver->init(0, nullptr);
  res = driver->devmgr_register();
  if (res != eno::ENO_OK) { return res; }
  return 0;
}

// static auto _dummyy_xxx __attribute__((section(".initcall0.init"))) = entry;
lvl0_initcall(entry);
