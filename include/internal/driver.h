#pragma once

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <internal/device.h>
#include <internal/devmgr.h>
#include <internal/dops.h>
#include <internal/toolchain.h>




typedef __aligned struct driver_env
{
    const char* name;
    const char* compat;
    const char* _json;
} * driver_env_t;

typedef void * driver_data_t;

typedef struct driver
{
    driver_env_t d_env;
    driver_data_t d_data;
    driver_id_t d_self;
    driver_ops_t d_ops;
} * driver_t;

/**
 * get driver structure from driver_id_t
 * @param[in] id driver_id_t
 * @return driver_t
 * @note TODO(savent): need implement
 */
static inline driver_t
dri_by_id(driver_id_t id)
{
    return 0;
}

/**
 * get driver's driver_id_t
 * @param[in] dri driver_t
 * @return driver_id_t
 */
static inline driver_id_t
dri_id(driver_t dri)
{
    return dri ? dri->d_self : 0;
}

/**
 * check if driver is registered
 * @note if driver already registered, driver_id_t must not be 0
 * @return true as registered
 */
static inline bool
dri_is_registered(driver_t dri)
{
    return dri_id(dri) == 0;
}

/**
 * checkout if device and driver matched
 * @param[in] dri driver_t
 * @param[in] dev device_t
 * @return true as matched
 */
static inline bool
dri_match_device(driver_t dri, device_t dev)
{
    const char *now = NULL, *next = NULL, *compat = NULL;
    if (!dri || !dri->d_env || !dri->d_env->compat || !dev || !dev->d_env ||
        !dev->d_env->compat)
        return false;

    compat = dev->d_env->compat;
    next = dri->d_env->compat - 1;
    do {
        now = next + 1;
        next = strchr(now, '|');
        int letter = next ? next - now : strlen(now);
        if (!strncmp(now, compat, letter))
            return true;
    } while (next);
    return false;
}

/**
 * driver ops:: init
 * @brief initial driver
 * @param[in] dri driver_t
 * @param[in] argc param num
 * @param[in] argv param
 * @note dops_init should register driver inside
 * @return see enum eno
 */
static inline int
dri_init(driver_t dri, int argc, char** argv)
{
    int res = dri ? dops_init(dri->d_ops, argc, argv) : EINVALIDE;
    if (!res) {
        driver_id_t dri_id = register_driver(dri);
        dri->d_self = dri_id;
    }
    return res;
}

/**
 * driver ops:: deinit
 * @brief deinit driver
 * @param[in] dri driver_t
 * @return see enum eno
 */
static inline int
dri_deinit(driver_t dri)
{
    int res = dri ? dops_deinit(dri->d_ops) : EINVALIDE;
    if (!res) {
        unregister_driver(dri->d_self);
        dri->d_self = 0;
    }
    return res;
}

/**
 * drvier ops:: bind
 * @brief bind driver and device
 * @param[in] dri driver_t
 * @param[in] dev device_id_t
 * @return see enum eno
 */
static inline int
dri_bind(driver_t dri, device_t dev)
{
    if (!dri_match_device(dri, dev) && dev_is_binded(dev))
        return EDENY;
    int res = dri ? dops_bind(dri->d_ops, dev) : EINVALIDE;
    if (!res) {
        dev->d_driver = dri_id(dri);
        dev->_d_driver = dri;
    }
    return res;
}

/**
 * driver ops:: unbind
 * @brief unbind driver and device
 * @param[in] dri driver_t
 * @param[in] dev device_id_t
 * @return see enum eno
 */
static inline int
dri_unbind(driver_t dri, device_t dev)
{
    int res = dri ? dops_unbind(dri->d_ops, dev) : EINVALIDE;
    if (!res) {
        dri->d_self = 0;
    }
    return 0;
}

/**
 * driver ops:: open
 * @brief open device
 * @param[in] dri driver_t
 * @param[in] dev device_id_t
 * @param[in] flags open flags
 * @return see enum eno
 */
static inline int
dri_open(driver_t dri, device_t dev, int flags)
{
    return dri ? dops_open(dri->d_ops, dev, flags) : EINVALIDE;
}

/**
 * driver ops:: close
 * @brief close deivce
 * @param[in] dri driver_t
 * @param[in] dev device_id_t
 * @return see enum eno
 */
static inline int
dri_close(driver_t dri, device_t dev)
{
    return dri ? dops_close(dri->d_ops, dev) : EINVALIDE;
}

/**
 * driver ops:: write
 * @brief write to device
 * @param[in] dri driver_t
 * @param[in] dev device_id_t
 * @param[in] in buffer align as Byte
 * @param[in] size size
 * @return see enum eno
 */
static inline int
dri_write(driver_t dri, device_t dev, const void* in, size_t size)
{
    return dri ? dops_write(dri->d_ops, dev, in, size) : EINVALIDE;
}

/**
 * driver ops:: read
 * @brief read from device
 * @param[in] dri driver_t
 * @param[in] dev device_id_t
 * @param[out] out output buffer, align as Byte
 * @param[in] size
 * @return see enum eno
 */
static inline int
dri_read(driver_t dri, device_t dev, void* out, size_t size)
{
    return dri ? dops_read(dri->d_ops, dev, out, size) : EINVALIDE;
}

/**
 * driver ops:: transfer(read/write)
 * @brief read/write some bytes
 * @param[in] dri driver_t
 * @param[in] dev device_id_t
 * @param[in] in buffer to write
 * @param[in] in_size write bytes
 * @param[in] out buffer to read
 * @param[in] out_size read bytes
 * @note if in == nullptr or in_size == 0, no write
 * @note if out== nullptr or out_size == 0, no read
 * @return see enum eno
 */
static inline int
dri_transfer(driver_t dri,
             device_t dev,
             const void* in,
             size_t in_size,
             void* out,
             size_t out_size)
{
    return dri ? dops_transfer(dri->d_ops, dev, in, in_size, out, out_size)
               : EINVALIDE;
}

/**
 * driver ops:: ioctl
 * @brief ioctl for device
 * @param[in] dri driver_t
 * @param[in] dev device_id_t
 * @param[in] cmd function to cmd
 * @param in_out (in or out) buffer
 * @param size (in or out) buffer size
 * @return see enum eno
 */
static inline int
dri_ioctl(driver_t dri, device_t dev, uint32_t cmd, void* in_out, size_t* size)
{
    return dri ? dops_ioctl(dri->d_ops, dev, cmd, in_out, size) : EINVALIDE;
}

/**
 * driver ops:: select
 * @brief listen device event
 * @param[in] dri driver_t
 * @param[in] dev device_id_t
 * @param[in] flags listen evnet list
 * @param[in] timeout 0 means no wait, (size_t)-1 means wait forever
 * @return see enum eno
 */
static inline int
dri_select(driver_t dri, device_t dev, uint32_t flags, size_t timeout)
{
    return dri ? dops_select(dri->d_ops, dev, flags, timeout) : EINVALIDE;
}


