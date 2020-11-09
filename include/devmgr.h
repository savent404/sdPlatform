#pragma once

#include <internal/toolchain.h>
#include <internal/type.h>

__cbegin

/**
 * devmgr:: register device
 * @param[in] str deivce json string
 * @return device_id_t, 0 as failed
 */
static inline device_id_t
devmgr_register_device(const char* str)
{
    return 0;
}

/**
 * devmgr:: unregister device
 * @param[in] dev_id device_id_t
 * @return true as success
 */
static inline bool
devmgr_unregister_device(device_id_t dev_id)
{
    return 0;
}

/**
 * devmgr:: register driver
 * @param[in] str driver json string
 * @return driver_id_t, 0 as failed
 */
static inline driver_id_t
devmgr_register_driver(const char* str)
{
    return 0;
}

/**
 * devmgr:: unregister driver
 * @param[in] id drvier_id_t
 * @return true as success
 */
static inline bool
devmgr_unregister_driver(driver_id_t dri_id)
{
    return 0;
}

/**
 * devmgr:: query device info
 * @param[in] dev_id device_id_t
 * @return json string, NULL as failed
 */
static inline const char*
devmgr_query_device(device_id_t dev_id)
{
    return "{}";
}

/**
 * devmgr:: query driver info
 * @param[in] dri_id driver_id_t
 * @return json string, NULL as failed
 */
static inline const char*
devmgr_query_driver(driver_id_t dri_id)
{
    return "{}";
}

/**
 * devmgr:: update json string of device
 * @param[in] id device_id_t
 * @param[in] str json string
 * @return true as success
 */
static inline bool
devmgr_update_device(device_id_t id, const char* str)
{
    return 0;
}

/**
 * devmgr:: update json string of driver
 * @param[in] id device_id_t
 * @param[in] str json string
 * @return true as success
 */
static inline int
devmgr_update_driver(driver_id_t id, const char* str)
{
    return 0;
}

__cend
