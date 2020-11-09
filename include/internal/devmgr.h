#pragma once

#include <devmgr.h>
#include <internal/device.h>
#include <internal/driver.h>
#include <internal/toolchain.h>
#include <internal/type.h>

__cbegin

static inline const char*
parse_device_to_json(device_t dev)
{
    return "{}";
}
static inline const char*
parse_driver_to_json(driver_t dri)
{
    return "{}";
}

static inline device_t
parse_device_from_json(const char* p)
{
    return NULL;
}
static inline driver_t
parse_driver_from_json(const char* p)
{
    return NULL;
}

static inline device_id_t
register_device(device_t dev)
{
    const char* str = parse_device_to_json(dev);
    return devmgr_register_device(str);
}

static inline driver_id_t
register_driver(driver_t dri)
{
    const char* str = parse_driver_to_json(dri);
    return devmgr_register_device(str);
}

static inline int
unregister_device(device_t dev)
{
    return devmgr_unregister_device(dev_id(dev));
}

static inline int
unregister_driver(driver_t dri)
{
    return devmgr_unregister_driver(dev_id(dri));
}

static inline device_t
query_device(device_id_t dev_id)
{
    const char* str = devmgr_query_device(dev_id);
    device_t dev = parse_device_from_json(str);
    return dev;
}

static inline driver_t
query_driver(driver_id_t dri_id)
{
    const char* str = devmgr_query_driver(dri_id);
    driver_t dri = parse_driver_from_json(str);
    return dri;
}

static inline int
update_device(device_t dev)
{
    const char* str = parse_device_to_json(dev);
    return devmgr_update_device(dev_id(dev), str);
}

/*
static inline int
update_driver(driver_t dri)
{
    const char* str = parse_driver_to_json(dri);
    return devmgr_update_driver(dri_id(dri), str);
}
*/

__cend
