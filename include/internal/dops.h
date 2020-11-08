#pragma once

#include <internal/type.h>
#include <internal/toolchain.h>

typedef struct driver_ops
{
    int (*init)(int, char**);
    int (*deinit)();
    int (*bind)(device_id_t);
    int (*unbind)(device_id_t);
    int (*open)(device_id_t, int);
    int (*close)(device_id_t);
    int (*write)(device_id_t, const void*, size_t);
    int (*read)(device_id_t, void*, size_t);
    int (*transfer)(device_id_t, const void *, size_t, void *, size_t);
    int (*ioctl)(device_id_t, uint32_t cmd, void *in_out, size_t *size);
    int (*select)(device_id_t, uint32_t flag, size_t timeout);
} *driver_ops_t;


/**
 * driver ops:: init
 * @brief initial driver
 * @param ops[in] driver_ops_t
 * @param argc[in] param num
 * @param argv[in] param
 * @return see enum eno
 */
static inline int dops_init(driver_ops_t ops, int argc, char **argv) {
    return ops && ops->init ? ops->init(argc, argv) : EINVALIDE;
}

/**
 * driver ops:: deinit
 * @brief deinit driver
 * @param ops[in] driver_ops_t
 * @return see enum eno
 */
static inline dops_deinit(driver_ops_t ops) {
    return ops && ops->deinit ? ops->deinit() : EINVALIDE;
}

/**
 * drvier ops:: bind
 * @brief bind driver and device
 * @param ops[in] driver_ops_t
 * @param dev_id[in] device_id_t
 * @return see enum eno
 */
static inline dops_bind(driver_ops_t ops, device_id_t dev_id) {
    return ops && ops->bind ? ops->bind(dev_id) : EINVALIDE;
}

/**
 * driver ops:: unbind
 * @brief unbind driver and device
 * @param ops[in] driver_ops_t
 * @param dev_id[in] device_id_t
 * @return see enum eno
 */
static inline dops_unbind(driver_ops_t ops, device_id_t dev_id) {
    return ops && ops->unbind ? ops->unbind(dev_id) : EINVALIDE;
}

/**
 * driver ops:: open
 * @brief open device
 * @param ops[in] driver_ops_t
 * @param dev_id[in] device_id_t
 * @param flags[in] open flags
 * @return see enum eno
 */
static inline dops_open(driver_ops_t ops, device_id_t dev_id, int flags) {
    return ops && ops->open ? ops->open(dev_id, flags) : EINVALIDE;
}

/**
 * driver ops:: close
 * @brief close deivce
 * @param ops[in] driver_ops_t
 * @param dev_id[in] device_id_t
 * @return see enum eno
 */
static inline dops_close(driver_ops_t ops, device_id_t dev_id) {
    return ops && ops->close ? ops->close(dev_id) : EINVALIDE;
}

/**
 * driver ops:: write
 * @brief write to device
 * @param ops[in] driver_ops_t
 * @param dev_id[in] device_id_t
 * @param in[in] buffer align as Byte
 * @param size[in] size
 * @return see enum eno
 */
static inline dops_write(driver_ops_t ops, device_id_t dev_id, const void *in, size_t size) {
    return ops && ops->write ? ops->write(dev_id, in, size) : EINVALIDE;
}

/**
 * driver ops:: read
 * @brief read from device
 * @param ops[in] driver_ops_t
 * @param dev_id[in] device_id_t
 * @param out[out] output buffer, align as Byte
 * @param size[in]
 * @return see enum eno
 */
static inline dops_read(driver_ops_t ops, device_id_t dev_id, void *out, size_t size) {
    return ops && ops->read ? ops->read(dev_id, out, size) : EINVALIDE;
}

/**
 * driver ops:: transfer(read/write)
 * @brief read/write some bytes
 * @param ops[in] driver_ops_t
 * @param dev_id[in] device_id_t
 * @param in[in] buffer to write
 * @param in_size[in] write bytes
 * @param out[in] buffer to read
 * @param out_size[in] read bytes
 * @note if in == nullptr or in_size == 0, no write
 * @note if out== nullptr or out_size == 0, no read
 * @return see enum eno
 */
static inline dops_transfer(driver_ops_t ops, device_id_t dev_id, const void *in, size_t in_size, void *out, size_t out_size) {
    return ops && ops->transfer ? ops->transfer(dev_id, in, in_size, out, out_size) : EINVALIDE;
}

/**
 * driver ops:: ioctl
 * @brief ioctl for device
 * @param ops[in] driver_ops_t
 * @param dev_id[in] device_id_t
 * @param cmd[in] function to cmd
 * @param in_out (in or out) buffer
 * @param size (in or out) buffer size
 * @return see enum eno
 */
static inline dops_ioctl(driver_ops_t ops, device_id_t dev_id, uint32_t cmd, void *in_out, size_t *size) {
    return ops && ops->ioctl ? ops->ioctl(dev_id, cmd, in_out, size) : EINVALIDE;
}

/**
 * driver ops:: select
 * @brief listen device event
 * @param ops[in] driver_ops_t
 * @param dev_id[in] device_id_t
 * @param flags[in] listen evnet list
 * @param timeout[in] 0 means no wait, (size_t)-1 means wait forever
 * @return see enum eno
 */
static inline dops_select(driver_ops_t ops, device_id_t dev_id, uint32_t flags, size_t timeout) {
    return ops && ops->select ? ops->select(dev_id, flags, timeout) : EINVALIDE;
}
