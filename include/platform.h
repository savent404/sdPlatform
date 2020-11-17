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
int bind(int device_id, int driver_id);
int unbind(int device_id);

int open(int device_id, int flags);
int close(int device_id);

int transfer(int device_id, const void *in, size_t in_len, void * out, size_t out_len);
int write(int device_id, const void *in, size_t len);
int read(int device_id, void *out, size_t len);

int ioctl(int device_id, void *in_out, size_t *in_out_len, size_t buffer_max_len);
/**
 * @}
 */
#ifdef __cplusplus
}
#endif
