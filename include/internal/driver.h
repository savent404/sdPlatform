#pragma once

#include <internal/device-type.h>
#include <internal/dops.h>
#include <internal/driver-type.h>
#include <internal/toolchain.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * get driver structure from driver_id_t
 * @param[in] id driver_id_t
 * @return driver_t
 * @note TODO(savent): need implement
 */
driver_t dri_by_id(driver_id_t id);

/**
 * get driver's driver_id_t
 * @param[in] dri driver_t
 * @return driver_id_t
 */
driver_id_t dri_id(driver_t dri);

/**
 * get driver's driver data
 * @param[in] dri driver_t
 * @return void*
 */
driver_data_t dri_get_data(driver_t dri);

/**
 * get driver's internal api
 * @param[in] dri driver_t
 * @return driver_internal_api_t
 */
driver_internal_api_t dri_get_intapi(driver_t dri);

/**
 * set driver's driver data
 * @param[in] dri driver_t
 * @param[in] data driver_data_t
 * @return none
 */
void dri_set_data(driver_t dri, driver_data_t data);

/**
 * check if driver is registered
 * @note if driver already registered, driver_id_t must not be 0
 * @return true as registered
 */
static inline bool dri_is_registered(driver_t dri);

/**
 * checkout if device and driver matched
 * @param[in] dri driver_t
 * @param[in] dev device_t
 * @return true as matched
 */
static inline bool dri_match_device(driver_t dri, device_t dev);

/**
 * driver ops:: init
 * @brief initial driver
 * @param[in] dri driver_t
 * @param[in] argc param num
 * @param[in] argv param
 * @note dops_init should register driver inside
 * @return see enum eno
 */
int dri_init(driver_t dri, int argc, char** argv);

/**
 * driver ops:: deinit
 * @brief deinit driver
 * @param[in] dri driver_t
 * @return see enum eno
 */
int dri_deinit(driver_t dri);

/**
 * drvier ops:: bind
 * @brief bind driver and device
 * @param[in] dri driver_t
 * @param[in] dev device_id_t
 * @return see enum eno
 */
int dri_bind(driver_t dri, device_t dev);

/**
 * driver ops:: unbind
 * @brief unbind driver and device
 * @param[in] dri driver_t
 * @param[in] dev device_id_t
 * @return see enum eno
 */
int dri_unbind(driver_t dri, device_t dev);

/**
 * driver ops:: open
 * @brief open device
 * @param[in] dri driver_t
 * @param[in] dev device_id_t
 * @param[in] flags open flags
 * @return see enum eno
 */
int dri_open(driver_t dri, device_t dev, int flags);

/**
 * driver ops:: close
 * @brief close deivce
 * @param[in] dri driver_t
 * @param[in] dev device_id_t
 * @return see enum eno
 */
int dri_close(driver_t dri, device_t dev);

/**
 * driver ops:: write
 * @brief write to device
 * @param[in] dri driver_t
 * @param[in] dev device_id_t
 * @param[in] in buffer align as Byte
 * @param[in] size size
 * @return see enum eno
 */
int dri_write(driver_t dri, device_t dev, const void* in, size_t size);

/**
 * driver ops:: read
 * @brief read from device
 * @param[in] dri driver_t
 * @param[in] dev device_id_t
 * @param[out] out output buffer, align as Byte
 * @param[in] size
 * @return see enum eno
 */
int dri_read(driver_t dri, device_t dev, void* out, size_t size);

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
int dri_transfer(driver_t dri,
                    device_t dev,
                    const void* in,
                    size_t in_size,
                    void* out,
                    size_t out_size);

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
int dri_ioctl(driver_t dri,
                device_t dev,
                uint32_t cmd,
                void* in_out,
                size_t* size);

/**
 * driver ops:: select
 * @brief listen device event
 * @param[in] dri driver_t
 * @param[in] dev device_id_t
 * @param[in] flags listen evnet list
 * @param[in] timeout 0 means no wait, (size_t)-1 means wait forever
 * @return see enum eno
 */
int dri_select(driver_t dri, device_t dev, uint32_t flags, size_t timeout);

#ifdef __cplusplus
}
#endif
