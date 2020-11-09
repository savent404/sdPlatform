#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef size_t cpu_t;

typedef cpu_t driver_id_t;
typedef cpu_t device_id_t;

enum eno
{
    EOK = 0,
    EINVALIDE = -1,
    EDENY = -2,
};
