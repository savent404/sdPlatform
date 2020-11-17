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

int dev_ioctl(int device_id, void *in_out, size_t *in_out_len, size_t buffer_max_len);

int devmgr_create_device(const char *json_str);
int devmgr_update_device(int device_id, const char *json_str);
int devmgr_remove_device(int device_id);
const char *devmgr_query_driver(int driver_id);

int devmgr_create_driver(const char *json_str);
int devmgr_update_driver(int driver_id, const char *json_str);
int devmgr_remove_driver(int driver_id);
const char *devmgr_query_device(int device_id);
/**
 * @}
 */
#ifdef __cplusplus
}
#endif
