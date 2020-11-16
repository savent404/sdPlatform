#pragma once

#include <internal/device-type.h>
#include <cJSON.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief checkout if device is binded
 * @param[in] dev device_t
 * @return true as binded
 * @note if device is not binded, dev->d_data must be NULL
 */
bool
dev_is_binded(device_t dev);

/**
 * @brief checkout if device id registered
 * @param[in] dev device_t
 * @return true as registered
 * @note if device is not registered, dev->d_self must be 0
 */
bool
dev_is_registered(device_t dev);

/**
 * @brief get device structure from device_id
 * @param[in] dev_id device_id_t
 * @return device_t
 * @note TODO(savent): implement a method to get device struct
 */
device_t
dev_by_id(device_id_t devid);

/**
 * @brief get device's id
 * @return device_it_t
 */
device_id_t
dev_id(device_t dev);

/**
 * @brief return cJSON obj from d_env
 * @param[in] dev
 * @return cJSON*, NULL means failed
 */
cJSON*
dev_env_json(device_t dev);

/**
 * @brief return json string from d_env
 * @param[in] dev
 * @return string, NULL means failed
 */
const char*
dev_env_string(device_t dev);

/**
 * @brief parse dev env's json string to cJSON obj
 * @param[in] dev device_t
 * @note output see 'dev->d_env->_json_obj' and method 'dev_env_json'
 * @return true as success
 */
bool
dev_env_parse(device_t dev);

/**
 * @brief dump dev env's cJSON obj to json string
 * @param[in] dev device_t
 * @note output see 'dev->d_env->_json_str' and method 'dev_env_string'
 * @return true as success
 */
bool
dev_env_dump(device_t dev);

/**
 * @brief get driver id of this device
 * @param dev device_t
 * @return driver_id_t
 */
 driver_id_t
dev_get_driver_id(device_t dev);

/**
 * @brief set driver id of this device
 * @param dev device_t
 * @param dri_id driver_id_t
 * @return none
 */
 driver_id_t
dev_set_driver_id(device_t dev, driver_id_t dri_id);

/**
 * @brief get driver's instance of this device
 * @param dev device_t
 * @warning only works in the same driver process.
 * @return device_t
 */
void*
dev_get_driver(device_t dev);

/**
 * @brief get driver's instance of this device
 * @param dev device_t
 * @param dri driver_t
 * @warning only works in the same driver process.
 * @return none
 */
void
dev_set_driver(device_t dev,void* dri);

/**
 * @brief get driver's member 'data'
 * @param dev device_t
 * @warning only works in the same driver process.
 * @return none
 */
void*
dev_get_data(device_t dev);

/**
 * @brief set driver's member 'data'
 * @param dev device_t
 * @warning only works in the same driver process.
 * @return device_t
 */
void
dev_set_data(device_t dev, device_data_t data);

#ifdef __cplusplus
}
#endif
