#pragma once

#include <internal/toolchain.h>
#include <stddef.h>
#include <stdint.h>
#include <type.h>

#ifdef __cplusplus
extern "C"
{
#endif

typedef __aligned struct driver_env
{
    const char* name;
    const char* compat;
    const char* _json;
} * driver_env_t;

typedef void* driver_internal_api_t;
typedef void* driver_data_t;
typedef struct driver_ops* driver_ops_t;

typedef struct driver
{
    driver_env_t d_env;
    driver_data_t d_data;
    driver_id_t d_self;
    driver_ops_t d_ops;
    driver_internal_api_t int_api;
} * driver_t;

#ifdef __cplusplus
}
#endif
