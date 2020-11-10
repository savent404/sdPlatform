#pragma once

#include <internal/toolchain.h>
#include <type.h>
#include <internal/device.h>



typedef struct driver_ops
{
    int (*init)(int, char**);
    int (*deinit)(void);
    int (*bind)(device_t);
    int (*unbind)(device_t);
    int (*open)(device_t, int);
    int (*close)(device_t);
    int (*write)(device_t, const void*, size_t);
    int (*read)(device_t, void*, size_t);
    int (*transfer)(device_t, const void*, size_t, void*, size_t);
    int (*ioctl)(device_t, uint32_t cmd, void* in_out, size_t* size);
    int (*select)(device_t, uint32_t flag, size_t timeout);
} * driver_ops_t;

/**
 * driver ops:: init
 * @brief initial driver
 * @param[in] ops driver_ops_t
 * @param[in] argc param num
 * @param[in] argv param
 * @return see enum eno
 */
static inline int
dops_init(driver_ops_t ops, int argc, char** argv)
{
    return ops && ops->init ? ops->init(argc, argv) : EINVALIDE;
}

/**
 * driver ops:: deinit
 * @brief deinit driver
 * @param[in] ops driver_ops_t
 * @return see enum eno
 */
static inline int
dops_deinit(driver_ops_t ops)
{
    return ops && ops->deinit ? ops->deinit() : EINVALIDE;
}

/**
 * drvier ops:: bind
 * @brief bind driver and device
 * @param[in] ops driver_ops_t
 * @param[in] dev device_t
 * @return see enum eno
 */
static inline int
dops_bind(driver_ops_t ops, device_t dev)
{
    return ops && ops->bind ? ops->bind(dev) : EINVALIDE;
}

/**
 * driver ops:: unbind
 * @brief unbind driver and device
 * @param[in] ops driver_ops_t
 * @param[in] dev device_t
 * @return see enum eno
 */
static inline int
dops_unbind(driver_ops_t ops, device_t dev)
{
    return ops && ops->unbind ? ops->unbind(dev) : EINVALIDE;
}

/**
 * driver ops:: open
 * @brief open device
 * @param[in] ops driver_ops_t
 * @param[in] dev device_t
 * @param[in] flags open flags
 * @return see enum eno
 */
static inline int
dops_open(driver_ops_t ops, device_t dev, int flags)
{
    return ops && ops->open ? ops->open(dev, flags) : EINVALIDE;
}

/**
 * driver ops:: close
 * @brief close deivce
 * @param[in] ops driver_ops_t
 * @param[in] dev device_t
 * @return see enum eno
 */
static inline int
dops_close(driver_ops_t ops, device_t dev)
{
    return ops && ops->close ? ops->close(dev) : EINVALIDE;
}

/**
 * driver ops:: write
 * @brief write to device
 * @param[in] ops driver_ops_t
 * @param[in] dev device_t
 * @param[in] in buffer align as Byte
 * @param[in] size size
 * @return see enum eno
 */
static inline int
dops_write(driver_ops_t ops, device_t dev, const void* in, size_t size)
{
    return ops && ops->write ? ops->write(dev, in, size) : EINVALIDE;
}

/**
 * driver ops:: read
 * @brief read from device
 * @param[in] ops driver_ops_t
 * @param[in] dev device_t
 * @param[out] out output buffer, align as Byte
 * @param[in] size
 * @return see enum eno
 */
static inline int
dops_read(driver_ops_t ops, device_t dev, void* out, size_t size)
{
    return ops && ops->read ? ops->read(dev, out, size) : EINVALIDE;
}

/**
 * driver ops:: transfer(read/write)
 * @brief read/write some bytes
 * @param[in] ops driver_ops_t
 * @param[in] dev device_t
 * @param[in] in buffer to write
 * @param[in] in_size write bytes
 * @param[in] out buffer to read
 * @param[in] out_size read bytes
 * @note if in == nullptr or in_size == 0, no write
 * @note if out== nullptr or out_size == 0, no read
 * @return see enum eno
 */
static inline int
dops_transfer(driver_ops_t ops,
              device_t dev,
              const void* in,
              size_t in_size,
              void* out,
              size_t out_size)
{
    return ops && ops->transfer
             ? ops->transfer(dev, in, in_size, out, out_size)
             : EINVALIDE;
}

/**
 * driver ops:: ioctl
 * @brief ioctl for device
 * @param[in] ops driver_ops_t
 * @param[in] dev device_t
 * @param[in] cmd function to cmd
 * @param in_out (in or out) buffer
 * @param size (in or out) buffer size
 * @return see enum eno
 */
static inline int
dops_ioctl(driver_ops_t ops,
           device_t dev,
           uint32_t cmd,
           void* in_out,
           size_t* size)
{
    return ops && ops->ioctl ? ops->ioctl(dev, cmd, in_out, size)
                             : EINVALIDE;
}

/**
 * driver ops:: select
 * @brief listen device event
 * @param[in] ops driver_ops_t
 * @param[in] dev device_t
 * @param[in] flags listen evnet list
 * @param[in] timeout 0 means no wait, (size_t)-1 means wait forever
 * @return see enum eno
 */
static inline int
dops_select(driver_ops_t ops,
            device_t dev,
            uint32_t flags,
            size_t timeout)
{
    return ops && ops->select ? ops->select(dev, flags, timeout) : EINVALIDE;
}


