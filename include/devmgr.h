#pragma once

#include <internal/toolchain.h>
#include <internal/type.h>
// #include <internal/driver.h>
// #include <internal/device.h>

__cbegin

static inline device_id_t
devmgr_register_device(const char* str)
{
    return 0;
}

static inline int
devmgr_unregister_device(device_id_t dev_id)
{
    return 0;
}

static inline driver_id_t
devmgr_register_driver(const char* str)
{
    return 0;
}

static inline int
devmgr_unregister_driver(driver_id_t dri_id)
{
    return 0;
}

static inline const char*
devmgr_query_device(device_id_t dev_id)
{
    return "{}";
}

static inline const char*
devmgr_query_driver(driver_id_t dri_id)
{
    return "{}";
}

static inline int
devmgr_update_device(device_id_t id, const char* str)
{
    return 0;
}

/*
static inline int
devmgr_update_driver(driver_id_t id, const char* str)
{
    return 0;
}
*/

__cend
