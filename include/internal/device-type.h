#pragma once

#include <internal/driver-type.h>
#include <internal/toolchain.h>
#include <stddef.h>
#include <stdint.h>
#include <type.h>

#ifdef __cplusplus
extern
{
#endif

// treat this as a json object
typedef struct device_env
{
    const char* name;
    const char* compat;
    const char* _json_str;
    void* _json_obj;
} * device_env_t;

typedef void* device_data_t;

typedef struct device
{
    device_env_t d_env;   // device init info
    device_data_t d_data; // device runtime info
    driver_id_t d_driver; // id to driver
    driver_t _d_driver;   // opinter to driver

    device_id_t d_self;
    device_id_t d_parent;
    device_id_t d_child;
    device_id_t d_prev;
    device_id_t d_next;
} * device_t;

#ifdef __cplusplus
}
#endif
