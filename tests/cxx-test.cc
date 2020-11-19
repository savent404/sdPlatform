// Copyright 2020 jrlc
#include <platform-types.h>
#include <platform.h>

#include <internal/driver.hxx>

namespace platform {
struct driver_demo : public driver {
 public:
  driver_demo() : driver({{"name", "demo"}, {"compat", "x86,demo|arm,demo"}}, nullptr) {}

 protected:
  virtual int init_(int argc, char** argv) { return eno::ENO_INVALID; }
  virtual int deinit_() { return eno::ENO_INVALID; }
  virtual int bind_(device_ref dev) { return eno::ENO_INVALID; }
  virtual int unbind_(device_ref dev) { return eno::ENO_INVALID; }
  virtual int open_(device_ref dev, int flags) { return eno::ENO_INVALID; }
  virtual int close_(device_ref dev) { return eno::ENO_INVALID; }
  virtual int transfer_(device_ref dev, const void* in, size_t in_len, void* out, size_t out_len) {
    return eno::ENO_INVALID;
  }
  virtual int write_(device_ref dev, const void* in, size_t len) { return eno::ENO_INVALID; }
  virtual int read_(device_ref dev, void* out, size_t len) { return eno::ENO_INVALID; }
  virtual int ioctl_(device_ref dev, int cmds, void* in_out, size_t* in_out_len, size_t buffer_max) {
    return eno::ENO_INVALID;
  }
};

}  // namespace platform

int main(void) {
  platform::driver_demo demo_driver;
  platform::device demo_device({{"name", "demo"}, {"compat", "arm,demo|none"}});

  int dev_id = demo_device.devmgr_register();
  int dri_id = demo_driver.devmgr_register();

  return dev_bind(dev_id, dri_id);
}
