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
    driver_internal_api_t d_intapi;
} * driver_t;

/**
 * @brief  delcear driver
 * @param[in][optional] name driver's name
 * @param[in]           compat driver's compat string
 * @param[in]           file_ops driver_ops_t
 * @param[in][optional] internal_api driver_internal_api_t
 */
/*
#define DECLEAR_DRIVER(name, compat, file_ops, internal_api)                   \
    {                                                                          \
        satic struct driver_env env = { \
            .name = name, \
            .compat = compat, \
        }; \
        static struct driver_##name = {                                        \
            .d_env = NULL,                                                     \
            .d_data = NULL,                                                    \
            .d_self = 0,                                                       \
            .d_ops = file_ops,                                                 \
            .d_intapi = internal_api,                                          \
        }; \
}
*/

#ifdef __cplusplus
}
#endif
