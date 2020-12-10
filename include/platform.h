// Copyright 2020 jrlc
#pragma once

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif
/**
 * @brief user API
 * @defgroup user_api
 * @{
 */
int dev_bind(int device_id, int driver_id);
int dev_unbind(int device_id);

int dev_open(int device_id, int flags);
int dev_close(int device_id);

int dev_transfer(int device_id, const void *in, size_t in_len, void *out, size_t out_len);
int dev_write(int device_id, const void *in, size_t len);
int dev_read(int device_id, void *out, size_t len);

int dev_ioctl(int device_id, int cmds, void *in_out, size_t in_len, size_t out_max_len);

/**
 * @}
 */
#ifdef __cplusplus
}
#endif
