/**
 * @file driver-dymmy.hxx
 * @author savent (savent_gate@outlook.com)
 * @brief
 * @version 0.1
 * @date 2020-11-20
 *
 * Copyright 2020 jrlc
 *
 */
#pragma once

#include <platform-types.h>
#include <string.h>

#include <memory>
#include <platform/driver.hxx>
#include <platform/entry.hxx>

namespace platform {

/**
 * @brief dummy驱动类型，用于序列号driver信息无实际的驱动功能
 *
 */
struct driver_dummy : public driver {
 public:
  using driver_ptr = std::unique_ptr<driver_dummy>;
  using driver_ref = driver_dummy&;

  driver_dummy() : driver({{"name", "dummy"}, {"compat", "dummy"}}, nullptr) {}
  ~driver_dummy() = default;

  /**
   * @brief 获取ipc通讯句柄
   * 
   * @return mx_channel_t* 
   */
  mx_channel_t* get_ipc_remote_channel() { return ipc_desc_.ch; }

 protected:
  virtual int init_(int argc, char** argv) { return eno::ENO_OK; }
  virtual int deinit_() { return eno::ENO_OK; }
  virtual int bind_(device_ref dev) { return eno::ENO_OK; }
  virtual int unbind_(device_ref dev) { return eno::ENO_OK; }
  virtual int open_(device_ref dev, int flags) { return eno::ENO_OK; }
  virtual int close_(device_ref dev) { return eno::ENO_OK; }
  virtual int transfer_(device_ref dev, const void* in, size_t in_len, void* out, size_t out_len) {
    memcpy(out, "world", out_len > 6 ? 6 : out_len);
    char* p = (char*)out;  // NOLINT
    p[out_len] = '\0';
    return eno::ENO_OK;
  }
  virtual int write_(device_ref dev, const void* in, size_t len) { return eno::ENO_OK; }
  virtual int read_(device_ref dev, void* out, size_t len) {
    memcpy(out, "world", len > 6 ? 6 : len);
    char* p = (char*)out;  // NOLINT
    p[len] = '\0';
    return eno::ENO_OK;
  }
  virtual int ioctl_(device_ref dev, int cmds, const void* in, size_t in_len, void* out, size_t* out_len,
                     size_t buffer_max) {
    int res = eno::ENO_INVALID;
    switch (cmds) {
      case 0:
        if (!strcmp((const char*)in, "Hello")) {
          memcpy(out, "world", buffer_max > 6 ? 6 : buffer_max);
          char* p = (char*)out;  // NOLINT
          p[buffer_max] = '\0';
          res = eno::ENO_OK;
        }
        break;
      default:
        res = eno::ENO_INVALID;
    }
    return res;
  }
  virtual void to_json_(cJSON* obj) {}
  virtual void from_json_(cJSON* obj) {
    if (cJSON_HasObjectItem(obj, "ipc")) {
      ipc_desc_.from_json(cJSON_GetObjectItem(obj, "ipc"));
    }
  }
  virtual void register_internal_syscall_() { ; }

 private:
  entry::ipc_desc ipc_desc_;
};

}  // namespace platform
