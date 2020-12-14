/**
 * @file syscall.hxx
 * @author savent (savent_gate@outlook.com)
 * @brief 提供内部系统调用方法
 * @version 0.1
 * @date 2020-11-20
 *
 * Copyright 2020 jrlc
 *
 */
#pragma once

#include <loop.h>

#include <cstddef>
#include <functional>
#include <memory>
#include <utility>

// clang-format off
#include <consthash/crc32.hxx>
#include <platform/alter/map.hxx>
#include <platform/alter/string.hxx>
#include <platform/cJSON.hxx>
#include <platform/syscall-details.hxx>
// clang-format on

namespace platform {

struct syscall {
 public:
  using func_t = syscall_dtl::func_t;
  using hash_id = size_t;
  using string = alter::string;  // NOLINT
  struct _msg_buf_t {
   public:
    _msg_buf_t();
    _msg_buf_t(_msg_buf_t&& other);
    ~_msg_buf_t();
    void* get();
    size_t size();

   private:
    friend class syscall;
    void set(char* ptr, size_t sz);
    void free_buffer();
    std::unique_ptr<char[]> buffer;
    size_t sz_;
  };

  struct ipc_desc {
    ipc_desc() = default;
    ipc_desc(const ipc_desc& other) = default;
    cJSON* to_json() const;
    void from_json(cJSON* obj);
    mx_channel_t* ch;
  };
  using msg_buf_t = std::unique_ptr<_msg_buf_t>;

 public:
  syscall() = default;
  ~syscall() = delete;

  bool init();
  bool deinit();

  /**
   * @brief 注册驱动调用
   *
   * @param func_name 驱动调用的名称, 如'open', 'close'
   * @param func func_t
   * @return true
   * @return false
   */
  bool add(string func_name, func_t func);
  /**
   * @brief 注销驱动调用
   *
   * @param func_name 驱动调用的名称, 如'open', 'close'
   * @return true
   * @return false
   */
  bool del(string func_name);

  /**
   * @brief 计算驱动调用的hash值
   *
   * @param func_name 驱动调用的名称, 如'open', 'close'
   * @return hash_id
   */
  static hash_id hash(const string& func_name) { return consthash::crc32(func_name.data(), func_name.length()); }
  constexpr static hash_id hash(const char* func_name) { return consthash::crc32(func_name, _strlen(func_name)); }

  /**
   * @brief 调用驱动调用
   *
   * @param id 驱动调用的hash值
   * @param buf 消息
   * @param len 消息长度
   * @param out 返回的消息 nullptr则不需要返回
   * @return int
   */
  int call(hash_id id, void* buf, size_t len, msg* out = nullptr) const;
  /**
   * @brief 调用驱动调用
   *
   * @param buf 消息(已包含驱动调用的hash值)
   * @param len 消息长度
   * @param out 返回的消息 nullptr则不需要返回
   * @return int
   */
  int call(void* buf, size_t len, msg* out = nullptr) const;

  template <typename... Args>
  int call_p(hash_id id, Args&... args) const;

  /**
   * @brief 获取syscall的单实例
   *
   * @return syscall*
   */
  static syscall* get_instance();

  /**
   * @brief 获取外部进程与本进程（驱动进程）通讯的ipc句柄
   *
   * @return const ipc_desc
   */
  ipc_desc get_local_ipc();

  /**
   * @brief 设置本地方法的ipc handle
   *
   * @param param ipc_desc
   */
  void set_local_ipc(ipc_desc param);
  /**
   * @brief 获取devmgr的ipc句柄
   *
   * @return const ipc_desc
   */
  ipc_desc get_devmgr_ipc();
  /**
   * @brief 设置devmgr的ipc handle
   *
   * @param param ipc_desc
   */
  void set_devmgr_ipc(ipc_desc param);
  /**
   * @brief 打包消息为msg_buf_t
   *
   * @tparam Args
   * @param[out] out_size 打包数据的大小
   * @param cmd ipc 调用 id
   * @param args 参数信息
   * @return msg_buf_t
   */
  template <typename... Args>
  static msg_buf_t package_msg(size_t* out_size, hash_id cmd, Args... args) {
    auto buf = new _msg_buf_t;
    auto res = syscall_dtl::package_param_to_buffer(out_size, cmd, args...);
    buf->set(res);
    return std::unique_ptr<_msg_buf_t>(buf);
  }
  /**
   * @brief 打包消息为msg_buf_t
   *
   * @tparam Args
   * @param args
   * @note args中第一个默认为 cmd 参数
   * @return msg_buf_t
   */
  template <typename... Args>
  static msg_buf_t package_msg(Args... args) {
    auto buf = new _msg_buf_t;
    size_t sz;
    auto res = syscall_dtl::package_param_to_buffer(&sz, args...);
    buf->set(res, sz);
    return std::unique_ptr<_msg_buf_t>(buf);
  }

 private:
  func_t find(hash_id func_id) const;
  bool add(hash_id func_id, func_t func);
  bool del(hash_id func_id);
  constexpr static int _strlen(const char* name) { return *name ? _strlen(name + 1) + 1 : 0; }
  ipc_desc local_ipc_;
  ipc_desc devmgr_ipc_;

  using kv_t = alter::map<hash_id, func_t>;
  kv_t kv_;
};

}  // namespace platform
