/**
 * @file device.cxx
 * @author savent (savent_gate@outlook.com)
 * @brief
 * @version 0.1
 * @date 2020-11-17
 *
 * Copyright 2020 jrlc
 *
 */
#include <internal/device.hxx>

namespace platform {

device::device() : runtime_(nullptr), config_({}) {}

device::device(parameters_init_list list) : runtime_(nullptr), config_(list) {}

cJSON* device::to_json() { return nullptr; }
void device::from_json(cJSON*) { return; }

cJSON* device::gen_patch(cJSON* from) { return nullptr; }
cJSON* device::apply_patch(cJSON* from) { return nullptr; }

device::runtime_ref device::get_runtime() { return *runtime_; }
device::parameters_ref device::get_config() { return config_; }

}  // namespace platform
