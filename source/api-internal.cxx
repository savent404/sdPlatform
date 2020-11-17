/**
 * @file api.cxx
 * @author savent (savent_gate@outlook.com)
 * @brief
 * @version 0.1
 * @date 2020-11-17
 *
 * Copyright 2020 jrlc
 *
 */
#include <platform.h>

#ifndef __weak
#define __weak __attribute__((weak))
#endif

#if 1

#include <map>
#include <thread> //NOLINT

#include <internal/driver.hxx>
#include <internal/device.hxx>

static std::map<int, platform::device*> device_queue;
static std::map<int, platform::driver*> driver_queue;

// __attribute__((constructor)) void devmanager_init(void)
// {

// }

extern "C" {

__weak int dev_bind(int device_id, int driver_id);
__weak int dev_unbind(int device_id);

__weak int dev_open(int device_id, int flags);
__weak int dev_close(int device_id);

__weak int dev_transfer(int device_id, const void *in, size_t in_len, void *out, size_t out_len);
__weak int dev_write(int device_id, const void *in, size_t len);
__weak int dev_read(int device_id, void *out, size_t len);

__weak int dev_ioctl(int device_id, void *in_out, size_t *in_out_len, size_t buffer_max_len);

__weak int devmgr_create_device(const char *json_str);
__weak int devmgr_update_device(int device_id, const char *json_str);
__weak int devmgr_remove_device(int device_id);
__weak const char *devmgr_query_driver(int driver_id);

__weak int devmgr_create_driver(const char *json_str);
__weak int devmgr_update_driver(int driver_id, const char *json_str);
__weak int devmgr_remove_driver(int driver_id);
__weak const char *devmgr_query_device(int device_id);
}

#else

extern "C" {

__weak int dev_bind(int device_id, int driver_id);
__weak int dev_unbind(int device_id);

__weak int dev_open(int device_id, int flags);
__weak int dev_close(int device_id);

__weak int dev_transfer(int device_id, const void *in, size_t in_len, void *out, size_t out_len);
__weak int dev_write(int device_id, const void *in, size_t len);
__weak int dev_read(int device_id, void *out, size_t len);

__weak int dev_ioctl(int device_id, void *in_out, size_t *in_out_len, size_t buffer_max_len);

__weak int devmgr_create_device(const char *json_str);
__weak int devmgr_update_device(int device_id, const char *json_str);
__weak int devmgr_remove_device(int device_id);
__weak const char *devmgr_query_driver(int driver_id);

__weak int devmgr_create_driver(const char *json_str);
__weak int devmgr_update_driver(int driver_id, const char *json_str);
__weak int devmgr_remove_driver(int driver_id);
__weak const char *devmgr_query_device(int device_id);
}

#endif
