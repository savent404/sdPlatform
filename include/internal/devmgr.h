#pragma once

#include <internal/type.h>
#include <internal/toolchain.h>
#include <internal/driver.h>
#include <internal/device.h>
#include <devmgr.h>

__cbegin


static inline device_id_t register_device(device_t dev) {
    // TODO(savent): format dev to json string
    const char *str = "{}";
    return devmgr_register_device(str);
}
static inline driver_id_t register_driver(driver_t dri) {
    // TODO(savent): format dri to json string
    const char *str = "{}";
    return devmgr_register_device(str);
}

static inline device_t query_device(device_id_t dev_id) {
    const char *str = devmgr_query_device(dev_id);
    // TODO(savent): parse json string to device_t
    device_t dev = NULL;
    return dev;
}

static inline driver_t query_driver(driver_id_t dri_id) {
    const char *str = devmgr_query_driver(dri_id);
    // TODO(savent): parse json string to driver_t
    driver_t dri = NULL;
    return dri;
}

__cend