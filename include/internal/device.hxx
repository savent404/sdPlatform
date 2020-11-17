/**
 * @file device.hxx
 * @author savent (savent_gate@outlook.com)
 * @brief
 * @version 0.1
 * @date 2020-11-17
 *
 * Copyright 2020 jrlc
 *
 */

#pragma once

#include <stddef.h>

#include <memory>

#include <internal/parameter.hxx>
#include <internal/runtime.hxx>
#include <internal/utils.hxx>

/**
 * @brief platform API
 * @defgroup platform_api
 * @{
 */
namespace platform {
struct device {
 public:
  using device_id = int;
  using driver_id = int;
  using value_ptr = std::unique_ptr<device>;
  using value_ref = device&;
  using parameters_ptr = parameters::value_ptr;
  using parameters_ref = parameters::value_ref;
  using parameters_init_list = parameters::initial_list;
  using runtime_ptr = runtime::value_ptr;
  using runtime_ref = runtime::value_ref;

  device();
  explicit device(parameters_init_list list);

  virtual cJSON* to_json();
  virtual void from_json(cJSON* obj);
  cJSON* gen_patch(cJSON* from);
  cJSON* apply_patch(cJSON* from);

 protected:
  runtime_ref get_runtime();
  parameters_ref get_config();

 private:
  runtime_ptr runtime_;
  parameters config_;
};
}  // namespace platform
/**
 * @}
 */
