/**
 * @file driver-dymmy.hxx
 * @author savent (savent_gate@outlook.com)
 * @brief
 * @version 0.1
 * @date 2020-11-20
 *
 * Copyright 2020 jrlc
 *
 */
#pragma once

#include <platform-types.h>

#include <platform/driver.hxx>

namespace platform {

struct driver_dummy : public driver {
 public:
  driver_dummy() : driver({{"name", "dummy"}, {"compat", "dummy"}}, nullptr) {}
  ~driver_dummy() = default;

 protected:
  virtual int init_(int argc, char** argv) { return eno::ENO_OK; }
  virtual int deinit_() { return eno::ENO_NOPERMIT; }
  virtual int bind_(device_ref dev) { return eno::ENO_OK; }
  virtual int unbind_(device_ref dev) { return eno::ENO_NOPERMIT; }
  virtual int open_(device_ref dev, int flags) { return eno::ENO_NOPERMIT; }
  virtual int close_(device_ref dev) { return eno::ENO_NOPERMIT; }
  virtual int transfer_(device_ref dev, const void* in, size_t in_len, void* out, size_t out_len) {
    return eno::ENO_NOPERMIT;
  }
  virtual int write_(device_ref dev, const void* in, size_t len) { return eno::ENO_NOPERMIT; }
  virtual int read_(device_ref dev, void* out, size_t len) { return eno::ENO_NOPERMIT; }
  virtual int ioctl_(device_ref dev, int cmds, void* in_out, size_t* in_out_len, size_t buffer_max) {
    return eno::ENO_NOPERMIT;
  }
  virtual cJSON* to_json_() { return nullptr; }
  virtual void from_json_(cJSON* obj) { ; }
  virtual void register_internal_syscall_() { ; }
};

}  // namespace platform
