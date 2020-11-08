#pragma once

#include <internal/type.h>
#include <internal/toolchain.h>
#include <internal/btree.h>
#include <internal/dops.h>

__cbegin

typedef void * device_data_t;

// treat this as a json object
typedef struct device_env
{
    void* _env;
} *device_env_t;

typedef struct device_data
{
    void* _data;
} *device_data_t;

typedef struct device
{
    device_env_t d_env;    // device init info
    device_data_t d_data;  // device runtime info
    driver_id_t d_driver;  // opinter to driver

    device_id_t d_self;
    device_id_t d_parent;
    device_id_t d_child;
    device_id_t d_prev;
    device_id_t d_next;
} *device_t;


/**
 * @brief checkout if device is binded
 * @param dev[in] device_t
 * @return true as binded
 * @note if device is not binded, dev->d_data must be NULL
 */
static inline bool dev_is_binded(device_t dev) {
    return dev && dev->d_data;
}

/**
 * @brief checkout if device id registered
 * @param dev[in] device_t
 * @return true as registered
 * @note if device is not registered, dev->d_self must be 0
 */
static inline bool dev_is_registered(device_t dev) {
    return dev->d_self == 0;
}

/**
 * @brief get device structure from device_id
 * @param dev_id[in] device_id_t
 * @return device_t
 * @note TODO(savent): implement a method to get device struct
 */
static inline device_t dev_by_id(device_id_t devid) {
    return NULL;
}

/**
 * @brief get device's id
 * @return device_it_t
 */
static inline device_id_t dev_id(device_t dev) {
    return dev && dev->d_self ? dev->d_self : NULL;
}
__cend
