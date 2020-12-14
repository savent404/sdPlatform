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
#include <utility>

// clang-format off
#include <platform/api.hxx>
#include <platform/driver.hxx>
#include <platform/entry.hxx>
#include <platform/remotecall.hxx>
#include <platform/syscall.hxx>
#include <platform/msg_wrapper.hxx>
// clang-format on

namespace platform {

/**
 * @brief dummy驱动类型，用于序列号driver信息无实际的驱动功能
 *
 */
struct driver_dummy : public driver, public dev_api {
 public:
  using driver_ptr = std::unique_ptr<driver_dummy>;
  using driver_ref = driver_dummy&;

  driver_dummy() : driver({{"name", "dummy"}, {"compat", "dummy"}}, nullptr) {}
  ~driver_dummy() = default;

  virtual int dev_bind(int dev_id, int dri_id) {
    auto hash = gen_hash("bind");
    msg msgbuf = msg_wrapper::package(hash, dev_id);
    auto client = get_rclient();
    return client._call(0, msgbuf.get(), msgbuf.size(), nullptr, nullptr);
  }
  virtual int dev_unbind(int dev_id) {
    auto hash = gen_hash("unbind");
    msg msgbuf = msg_wrapper::package(hash, dev_id);
    auto client = get_rclient();
    return client._call(0, msgbuf.get(), msgbuf.size(), nullptr, nullptr);
  }
  virtual int dev_open(int dev_id, int flags) {
    auto hash = gen_hash("open");
    msg msgbuf = msg_wrapper::package(hash, dev_id, flags);
    auto client = get_rclient();
    return client._call(0, msgbuf.get(), msgbuf.size(), nullptr, nullptr);
  }
  virtual int dev_close(int dev_id) {
    auto hash = gen_hash("close");
    msg msgbuf = msg_wrapper::package(hash, dev_id);
    auto client = get_rclient();
    return client._call(0, msgbuf.get(), msgbuf.size(), nullptr, nullptr);
  }
  virtual int dev_transfer(int dev_id, const void* in, size_t in_len, void* out, size_t out_len) {
    auto hash = gen_hash("close");
    msg_ref refmsg(out, out_len);
    auto msgbuf = msg_wrapper::package(hash, dev_id, in, in_len, &refmsg);
    auto client = get_rclient();
    size_t sz = refmsg.size();
    return client._call(0, msgbuf.get(), msgbuf.size(), refmsg.get(), &sz);
  }
  virtual int dev_write(int dev_id, const void* in, size_t len) {
    auto hash = gen_hash("write");
    auto msgbuf = msg_wrapper::package(hash, dev_id, in, len);
    auto client = get_rclient();
    return client._call(0, msgbuf.get(), msgbuf.size(), nullptr, nullptr);
  }
  virtual int dev_read(int dev_id, void* in, size_t len) {
    auto hash = gen_hash("read");
    msg_ref refmsg(in, len);
    auto msgbuf = msg_wrapper::package(hash, dev_id, &refmsg);
    auto client = get_rclient();
    size_t sz = refmsg.size();
    return client._call(0, msgbuf.get(), msgbuf.size(), refmsg.get(), &sz);
  }
  virtual int dev_ioctl(int dev_id, int cmds, void* in_out, size_t in_len, size_t buffer_max_len) {
    auto hash = gen_hash("ioctl");
    msg_ref refmsg(in_out, buffer_max_len);
    auto msgbuf = msg_wrapper::package(hash, dev_id, cmds, in_out, in_len, &refmsg);
    auto client = get_rclient();
    size_t sz = refmsg.size();
    return client._call(0, msgbuf.get(), msgbuf.size(), refmsg.get(), &sz);
  }

  syscall::hash_id gen_hash(const char *method) {
    platform::alter::string s;
    s.append(get_name());
    s.append("-");
    s.append(method);
    return syscall::hash(s);
  }
  /**
   * @brief 获取rclient 实例
   * 
   * @return rcall_client&& 
   */
  rcall_client&& get_rclient() { return std::move(rcall_client(ipc_desc_.ch)); }

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
