#pragma once

#include <devmgr.h>
#include <internal/device.h>
#include <internal/driver.h>
#include <internal/toolchain.h>
#include <internal/type.h>
#include <cJSON.h>

__cbegin


/**
 * convert device structure to json string
 * @note return instance came from cJSON_malloc, remenber call 'cJSON_free' to free it.
 * @param[in] dev device_t
 * @return json string, NULL as failed
 */
static inline const char*
parse_device_to_json(device_t dev)
{
    return "{}";
}

/**
 * convert driver structure to json string
 * @note return instance came from cJSON_malloc, remenber call 'cJSON_free' to free it.
 * @param[in] dri driver_t
 * @return json string, NULL as failed
 */
static inline const char*
parse_driver_to_json(driver_t dri)
{
    return "{}";
}

/**
 * convert json string to device structure
 * @note return instance came from cJSON_malloc, remenber call 'cJSON_free' to free it.
 * @param[in] p json string
 * @return device_t, NULL means failed
 */
static inline device_t
parse_device_from_json(const char* p)
{
    return NULL;
}

/**
 * convert json string to device structure
 * @note return instance came from cJSON_malloc, remenber call 'cJSON_free' to free it.
 * @param[in] p json string
 * @return driver_t, NULL means faield
 */
static inline driver_t
parse_driver_from_json(const char* p)
{
    return NULL;
}

/**
 * devmgr:: register device
 * @param[in] dev deivce_t
 * @return device_id_t, 0 as failed
 */
static inline device_id_t
register_device(device_t dev)
{
    const char* str = parse_device_to_json(dev);
    return devmgr_register_device(str);
}

/**
 * devmgr:: register driver
 * @param[in] dri driver_t
 * @return driver_id_t, 0 as failed
 */
static inline driver_id_t
register_driver(driver_t dri)
{
    const char* str = parse_driver_to_json(dri);
    return devmgr_register_device(str);
}

/**
 * devmgr:: unregister device
 * @param[in] dev deivce_t
 * @return true as success
 */
static inline bool
unregister_device(device_t dev)
{
    return devmgr_unregister_device(dev_id(dev));
}

/**
 * devmgr:: unregister driver
 * @param[in] dri driver_t
 * @return true as success
 */
static inline bool
unregister_driver(driver_t dri)
{
    return devmgr_unregister_driver(dri_id(dri));
}

/**
 * devmgr:: query device info
 * @param[in] dev deivce_t
 * @return json string, NULL as failed
 */
static inline device_t
query_device(device_id_t dev_id)
{
    const char* str = devmgr_query_device(dev_id);
    device_t dev = parse_device_from_json(str);
    return dev;
}

/**
 * devmgr:: query driver info
 * @param[in] dri driver_t
 * @return json string, NULL as failed
 */
static inline driver_t
query_driver(driver_id_t dri_id)
{
    const char* str = devmgr_query_driver(dri_id);
    driver_t dri = parse_driver_from_json(str);
    return dri;
}

/**
 * devmgr:: update json string of device
 * @param[in] dev deivce_t
 * @param[in] str json string
 * @return true as success
 */
static inline int
update_device(device_t dev)
{
    const char* str = parse_device_to_json(dev);
    return devmgr_update_device(dev_id(dev), str);
}

/**
 * devmgr:: update json string of driver
 * @param[in] dri driver_t
 * @param[in] str json string
 * @return true as success
 */
static inline int
update_driver(driver_t dri)
{
    const char* str = parse_driver_to_json(dri);
    return devmgr_update_driver(dri_id(dri), str);
}

__cend
