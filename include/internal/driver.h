#pragma once

#include <stddef.h>
#include <stdint.h>

#include <internal/device.h>
#include <internal/toolchain.h>

__cbegin

typedef struct driver_env
{

} * driver_env_t;

struct driver
{
    driver_env_t d_env;
} * driver_t;

__cend
