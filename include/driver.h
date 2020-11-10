#pragma once

#include <type.h>

/**
 * driver ops:: init
 * @brief initial driver
 * @param[in] id driver_id_t
 * @param[in] argc param num
 * @param[in] argv param
 * @return see enum eno
 */
int
driver_init(driver_id_t id, int argc, char** argv);

/**
 * driver ops:: deinit
 * @brief deinit driver
 * @param[in] id driver_id_t
 * @return see enum eno
 */
int
driver_deinit(driver_id_t id);

/**
 * drvier ops:: bind
 * @brief bind driver and device
 * @param[in] id driver_id_t
 * @param[in] dev device_id_t
 * @return see enum eno
 */
int
driver_bind(driver_id_t id, device_id_t dev);

/**
 * driver ops:: unbind
 * @brief unbind driver and device
 * @param[in] id driver_id_t
 * @param[in] dev device_id_t
 * @return see enum eno
 */
int
driver_unbind(driver_id_t id, device_id_t dev);

/**
 * driver ops:: open
 * @brief open device
 * @param[in] id driver_id_t
 * @param[in] dev device_id_t
 * @param[in] flags open flags
 * @return see enum eno
 */
int
driver_open(driver_id_t id, device_id_t dev, int flags);

/**
 * driver ops:: close
 * @brief close deivce
 * @param[in] id driver_id_t
 * @param[in] dev device_id_t
 * @return see enum eno
 */
int
driver_close(driver_id_t id, device_id_t dev);

/**
 * driver ops:: write
 * @brief write to device
 * @param[in] id driver_id_t
 * @param[in] dev device_id_t
 * @param[in] in buffer align as Byte
 * @param[in] size size
 * @return see enum eno
 */
int
driver_write(driver_id_t id, device_id_t dev, const void* in, size_t size);

/**
 * driver ops:: read
 * @brief read from device
 * @param[in] id driver_id_t
 * @param[in] dev device_id_t
 * @param[out] out output buffer, align as Byte
 * @param[in] size
 * @return see enum eno
 */
int
driver_read(driver_id_t id, device_id_t dev, void* out, size_t size);

/**
 * driver ops:: transfer(read/write)
 * @brief read/write some bytes
 * @param[in] id driver_id_t
 * @param[in] dev device_id_t
 * @param[in] in buffer to write
 * @param[in] in_size write bytes
 * @param[in] out buffer to read
 * @param[in] out_size read bytes
 * @note if in == nullptr or in_size == 0, no write
 * @note if out== nullptr or out_size == 0, no read
 * @return see enum eno
 */
int
driver_transfer(driver_id_t id,
                device_id_t dev,
                const void* in,
                size_t in_size,
                void* out,
                size_t out_size);

/**
 * driver ops:: ioctl
 * @brief ioctl for device
 * @param[in] id driver_id_t
 * @param[in] dev device_id_t
 * @param[in] cmd function to cmd
 * @param in_out (in or out) buffer
 * @param size (in or out) buffer size
 * @return see enum eno
 */
int
driver_ioctl(driver_id_t id,
             device_id_t dev,
             uint32_t cmd,
             void* in_out,
             size_t* size);
/**
 * driver ops:: select
 * @brief listen device event
 * @param[in] id driver_id_t
 * @param[in] dev device_id_t
 * @param[in] flags listen evnet list
 * @param[in] timeout 0 means no wait, (size_t)-1 means wait forever
 * @return see enum eno
 */
int
driver_select(driver_id_t id, device_id_t dev, uint32_t flags, size_t timeout);
