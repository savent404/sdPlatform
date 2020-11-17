#pragma once

#include <platform-types.h>

#ifdef __cplusplus
extern "C" {
#endif
/**
 * @brief user API
 * @defgroup user_api
 * @{
 */
enum eno
{
    EOK = 0,
    EINVALID = -1,
    ENOPERMIT = -2,
    EBUSY = -3,
    ENOTEXIST = -4,
};
/**
 * @}
 */
#ifdef __cplusplus
}
#endif
