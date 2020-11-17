/**
 * @file driver.hxx
 * @author savent (savent_gate@outlook.com)
 * @brief
 * @version 0.1
 * @date 2020-11-17
 *
 * Copyright 2020 jrlc
 *
 */
#pragma once

#include <cJSON.h>
#include <stddef.h>

#include <list>
#include <utility>

#include <internal/device.hxx>
#include <internal/parameter.hxx>
#include <internal/runtime.hxx>
#include <internal/utils.hxx>
/**
 * @brief platform API
 * @defgroup platform_api
 * @{
 */
namespace platform {

/**
 * @brief platform driver api
 */
struct driver {
 public:
  using driver_id = int;
  using device_id = device::device_id;
  using device_ptr = device::value_ptr;
  using device_ref = device::value_ref;
  using device_kv = std::pair<device_id, device_ptr>;
  using device_kv_list = std::list<device_kv>;
  using parameters_ptr = parameters::value_ptr;
  using parameters_ref = parameters::value_ref;
  using runtime_ptr = runtime::value_ptr;
  using runtime_ref = runtime::value_ref;

  explicit driver(parameters::initial_list config_list, runtime_ptr runtime);
  virtual ~driver() = default;

  int init(int argc, char** argv);
  int deinit();
  int bind(device_id dev);
  int unbind(device_id dev);
  int open(device_id dev, int flags);
  int close(device_id dev);
  int transfer(device_id dev, const void* in, size_t in_len, void* out, size_t out_len);
  int write(device_id dev, const void* in, size_t len);
  int read(device_id dev, void* out, size_t len);
  int ioctl(device_id dev, int cmds, void* in_out, size_t* in_out_len, size_t buffer_max);
  cJSON* toJson();
  // const char* serialize();

 protected:
  virtual int init_(int argc, char** argv) = 0;
  virtual int deinit_() = 0;
  virtual int bind_(device_ref dev) = 0;
  virtual int unbind_(device_ref dev) = 0;
  virtual int open_(device_ref dev, int flags) = 0;
  virtual int close_(device_ref dev) = 0;
  virtual int transfer_(device_ref dev, const void* in, size_t in_len, void* out, size_t out_len) = 0;
  virtual int write_(device_ref dev, const void* in, size_t len) = 0;
  virtual int read_(device_ref dev, void* out, size_t len) = 0;
  virtual int ioctl_(device_ref dev, int cmds, void* in_out, size_t* in_out_len, size_t buffer_max) = 0;
  virtual cJSON* toJson_() = 0;

  parameters_ref get_config();
  runtime_ref get_runtime();
  device_ptr query_device(device_id id);
  bool update_device(device_id id, device_ref dev);

 protected:
  parameters config_;
  runtime_ptr runtime_p_;

 private:
  bool add_device(device_id id, device_ptr ptr);
  bool has_device(device_id id);
  bool del_device(device_id id);
  device_ptr get_device(device_id id);
  bool put_device(device_id, device_ptr ptr);


 private:
  device_kv_list device_list_;
};
}  // namespace platform
/**
 * @}
 */
