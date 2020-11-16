/**
 * @file
 *
 * @brief Public APIs for the I2C bus drivers.
 */

#pragma once

#include <internal/toolchain.h>
#include <internal/device-type.h>
#include <cJSON.h>
#include <stdint.h>

#ifdef __cplusplus
ipcern "C"
{
#endif

/**
 * @brief I2C bus Interface
 * @defgroup i2c_bus_interface I2C Interface
 * @ingroup io_interfaces
 * @{
 */

/**
 * i2c bus frequence
 */
enum i2c_bus_speed
{
    // 100KHz
    I2C_BUS_SPEED_STANDARD = 0,
    // 400KHz
    I2C_SPEED_FAST,
    // 1MHz
    I2C_SPEED_FAST_PLUS,
    // 3.4MHz
    I2C_SPEED_HIGH,
    // 5MHz
    I2C_SPEED_ULTRA,
};

/**
 * i2c bus master/slave mode
 */
enum i2c_bus_mode
{
    I2C_BUS_MASTER_MODE = 0,
    I2C_BUS_SLAVE_MODE,
};

/**
 * i2c msg transmit/recive flags
 */
enum i2c_msg_flag
{
    I2C_MSG_FLAG_WRITE = BIT(0),
    I2C_MSG_FLAG_READ = BIT(1),
    I2C_MSG_STOP = BIT(2),
    I2C_MSG_RESTART = BIT(3),
    I2C_MSG_USE_10BIT_ADDR = BIT(4),
};

/**
 * @brief I2C Msg package
 *
 * This defines one I2C message to transact on the i2c bus
 *
 * @note some of the flags supported by this API may not be supported by
 * specific i2c hardware implementations.
 */
typedef struct i2c_msg
{
    uint8_t *buf;
    size_t len;
    uint8_t flag;
} *i2c_msg_t;


typedef struct i2c_bus_runtime
{
    void* _priv;
    enum i2c_bus_speed bus_speed;
    enum i2c_bus_mode  bus_mode;
} *i2c_bus_runtime_t;

typedef struct i2c_bus_api
{
    int (*init)(i2c_bus_runtime_t rt, cJSON* device_config);
    int (*deinit)(i2c_bus_runtime_t rt);
    int (*transfer)(i2c_bus_runtime_t rt, i2c_msg_t msg, size_t num_msg, uint16_t slave_addr);
    int (*reset)(i2c_bus_runtime_t rt);
} *i2c_bus_api_t;


int i2c_bus_transfer(device_t dev, i2c_msg_t msg, size_t num_msgs, uint16_t addr);
int i2c_bus_write(device_t dev, const uint8_t* data, size_t len);
int i2c_bus_read(device_t dev, uint8_t *data, size_t len);
int i2c_bus_read_write(device_t dev, const uint8_t *tx_data, size_t tx_len, uint8_t *rx_data, size_t rx_len);
int i2c_bus_reset(device_t dev);

int i2c_bus_transfer_ipc(device_id_t dev, i2c_msg_t msg, size_t num_msgs, uint16_t addr);
int i2c_bus_write_ipc(device_id_t dev, const uint8_t* data, size_t len);
int i2c_bus_read_ipc(device_id_t dev, uint8_t *data, size_t len);
int i2c_bus_read_write_ipc(device_id_t dev, const uint8_t *tx_data, size_t tx_len, uint8_t *rx_data, size_t rx_len);
int i2c_bus_reset_ipc(device_id_t dev);
/**
 * @}
 * i2c_bus_interface
 */
#ifdef __cplusplus
}
#endif
