/**
 * @file device.hxx
 * @author savent (savent_gate@outlook.com)
 * @brief 描述设备
 * @version 0.1
 * @date 2020-11-17
 *
 * Copyright 2020 jrlc
 *
 */
#pragma once

#include <stddef.h>

#include <memory>

#include <platform/parameter.hxx>
#include <platform/runtime.hxx>
#include <platform/utils.hxx>

namespace platform {
/**
 * @brief 设备类
 * @note 可将对外可见的数据序列化为json并上报给devmgr
 * @note 对外不可见的数据通常存储在其私有变量 runtime_中，仅由驱动进程维护
 */
struct device {
 public:
  using device_id = int;
  using driver_id = int;
  using device_ptr = std::unique_ptr<device>;
  using device_ref = device&;
  using parameters_ptr = parameters::value_ptr;
  using parameters_ref = parameters::value_ref;
  using parameters_init_list = parameters::initial_list;
  using runtime_ptr = runtime::value_ptr;
  using runtime_ref = runtime::value_ref;

  device();
  explicit device(parameters_init_list list);

  virtual cJSON* to_json();
  virtual void from_json(cJSON* obj);
  const char* to_json_str();
  void from_json_str(const char*);

  /**
   * @brief devmanager's api
   * @defgroup device_devmgr_api
   * @{
   */
  device_id devmgr_register();
  void devmgr_update();
  void devmgr_query();
  void devmgr_delete();
  /**
   * @}
   */

  runtime_ref get_runtime();
  void set_runtime(runtime_ptr ptr);

  parameters_ref get_config();

  device_id get_id();
  void set_id(device_id);

  driver_id get_bind_id();
  void set_bind_id(driver_id id);

 private:
  device_id id_;
  runtime_ptr runtime_;
  parameters config_;
};
}  // namespace platform
