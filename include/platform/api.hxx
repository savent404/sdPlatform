/**
 * @file api.hxx
 * @author savent (savent_gate@outlook.com)
 * @brief
 * @version 0.1
 * @date 2020-12-14
 *
 * Copyright 2020 jrlc
 *
 */
#pragma once

#include <cstddef>

namespace platform {

struct dev_api {
 public:
    virtual int dev_bind(int dev_id, int dri_id) = 0;
    virtual int dev_unbind(int dev_id) = 0;
    virtual int dev_open(int dev_id, int flags) = 0;
    virtual int dev_close(int dev_id) = 0;
    virtual int dev_transfer(int dev_id, const void* in, size_t in_len, void* out, size_t out_len) = 0;
    virtual int dev_write(int dev_id, const void* in, size_t len) = 0;
    virtual int dev_read(int dev_id, void* in, size_t len) = 0;
    virtual int dev_ioctl(int dev_id, int cmds, void* in_out, size_t in_len, size_t buffer_max_len) = 0;
};
}  // namespace platform
