#pragma once

#include <internal/driver-type.h>
#include <internal/device-type.h>
#include <internal/toolchain.h>
#include <cJSON.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * convert device structure to json string
 * @note return instance came from cJSON_malloc, remenber call 'cJSON_free' to free it.
 * @param[in] dev device_t
 * @return json string, NULL as failed
 */
const char*
parse_device_to_json(device_t dev);

/**
 * convert driver structure to json string
 * @note return instance came from cJSON_malloc, remenber call 'cJSON_free' to free it.
 * @param[in] dri driver_t
 * @return json string, NULL as failed
 */
const char*
parse_driver_to_json(driver_t dri);

/**
 * convert json string to device structure
 * @note return instance came from cJSON_malloc, remenber call 'cJSON_free' to free it.
 * @param[in] p json string
 * @return device_t, NULL means failed
 */
device_t
parse_device_from_json(const char* p);

/**
 * convert json string to device structure
 * @note return instance came from cJSON_malloc, remenber call 'cJSON_free' to free it.
 * @param[in] p json string
 * @return driver_t, NULL means faield
 */
driver_t
parse_driver_from_json(const char* p);

/**
 * devmgr:: register device
 * @param[in] dev deivce_t
 * @return device_id_t, 0 as failed
 */
static inline device_id_t
register_device(device_t dev);

/**
 * devmgr:: register driver
 * @param[in] dri driver_t
 * @return driver_id_t, 0 as failed
 */
driver_id_t
register_driver(driver_t dri);

/**
 * devmgr:: unregister device
 * @param[in] dev deivce_t
 * @return true as success
 */
bool
unregister_device(device_t dev);

/**
 * devmgr:: unregister driver
 * @param[in] dri driver_t
 * @return true as success
 */
bool
unregister_driver(driver_t dri);

/**
 * devmgr:: query device info
 * @param[in] dev deivce_t
 * @return json string, NULL as failed
 */
device_t
query_device(device_id_t dev_id);

/**
 * devmgr:: query driver info
 * @param[in] dri driver_t
 * @return json string, NULL as failed
 */
driver_t
query_driver(driver_id_t dri_id);

/**
 * devmgr:: update json string of device
 * @param[in] dev deivce_t
 * @param[in] str json string
 * @return true as success
 */
int
update_device(device_t dev);

/**
 * devmgr:: update json string of driver
 * @param[in] dri driver_t
 * @param[in] str json string
 * @return true as success
 */
int
update_driver(driver_t dri);

#ifdef __cplusplus
}
#endif
