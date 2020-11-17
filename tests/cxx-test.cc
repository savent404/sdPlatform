// Copyright 2020 jrlc
#include <platform-types.h>
#include <platform.h>

#include <iostream>

#include <internal/driver.hxx>

int foo1(void) {
  platform::parameters::initial_list list = {
      {"name", "foo"},
      {"compat", "x86,foo"},
      {"config/var1", true},
      {"config/var2", 12.0f},
  };
  platform::parameters data(list);

  std::cout << "name:" << data.has("name") << std::endl;
  std::cout << "compat:" << data.has("compat") << std::endl;
  std::cout << "config/var1: " << data.has("config/var1") << std::endl;
  std::cout << "config/var2: " << data.has("config/var2") << std::endl;
  std::cout << "config/var3: " << data.has("config/var3") << std::endl;

  return 0;
}

namespace platform {
struct driver_demo : public driver {
 public:
  driver_demo() : driver({{"name", "demo"}, {"compat", "x86,demo"}}, nullptr) {}

 protected:
  virtual int init_(int argc, char** argv) { return eno::EINVALID; }
  virtual int deinit_() { return eno::EINVALID; }
  virtual int bind_(device_ref dev) { return eno::EINVALID; }
  virtual int unbind_(device_ref dev) { return eno::EINVALID; }
  virtual int open_(device_ref dev, int flags) { return eno::EINVALID; }
  virtual int close_(device_ref dev) { return eno::EINVALID; }
  virtual int transfer_(device_ref dev, const void* in, size_t in_len, void* out, size_t out_len) {
    return eno::EINVALID;
  }
  virtual int write_(device_ref dev, const void* in, size_t len) { return eno::EINVALID; }
  virtual int read_(device_ref dev, void* out, size_t len) { return eno::EINVALID; }
  virtual int ioctl_(device_ref dev, int cmds, void* in_out, size_t* in_out_len, size_t buffer_max) {
    return eno::EINVALID;
  }

  virtual cJSON* toJson_() { return nullptr; }
};

}  // namespace platform

int main(void) {
  foo1();

  platform::driver_demo demo;
  demo.bind(0);
  return 0;
}
