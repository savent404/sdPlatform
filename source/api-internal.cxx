/**
 * @file api.cxx
 * @author savent (savent_gate@outlook.com)
 * @brief
 * @version 0.1
 * @date 2020-11-17
 *
 * Copyright 2020 jrlc
 *
 */
#include <platform-types.h>
#include <platform.h>
#include <requirements.h>

#include <map>
#include <memory>
#include <random>

// clang-format off
#include <platform/alter/map.hxx>
#include <platform/device.hxx>
#include <platform/driver-dummy.hxx>
#include <platform/driver.hxx>
#include <platform/syscall.hxx>
#include <platform/msg.hxx>
#include <platform/msg_wrapper.hxx>
#include <platform/remotecall.hxx>
// clang-format on

#ifndef __weak
#define __weak __attribute__((weak))
#endif

using syscall = platform::syscall;
using msg_wrapper = platform::msg_wrapper;
using msg = platform::msg;
using msg_ref = platform::msg_ref;

// 导入操作系统提供的free函数(用于free_devmgr_string函数)
extern "C" void heap_free(void *);
/**
 * @brief 释放由 devmgr 返回的字符串内存
 * 
 * @param str 字符串指针
 */
static void free_devmgr_string(const char *str) { heap_free(const_cast<char *>(str)); }

/**
 * @brief 获取驱动的缓存集
 * 
 * @note 使用缓存的意义在于每次系统调用到来不需要通过devmgr api查询相关信息
 * @return std::map<int, platform::driver_dummy::driver_ptr> &
 */
static auto &get_driver_map() {
  static platform::alter::map<int, platform::driver_dummy::driver_ptr> *driver_map;
  if (!driver_map) driver_map = new platform::alter::map<int, platform::driver_dummy::driver_ptr>;
  return *driver_map;
}

/**
 * @brief 获取设备的缓存集
 * 
 * @note 使用缓存的意义在于每次系统调用到来不需要通过devmgr api查询相关信息
 * @return std::map<int, platform::device::device_ptr> &
 */
static auto &get_device_map() {
  static platform::alter::map<int, platform::device::device_ptr> *device_map;
  if (!device_map) device_map = new platform::alter::map<int, platform::device::device_ptr>;
  return *device_map;
}

/**
 * @brief 查找合适的驱动
 * 
 * @note 优先从驱动缓存集中查询，查找不到则调用devmgr api
 * @param driver_id 驱动的id
 * @param item_iter 查找到的结果
 * @return int 0  成功
 *             !0 失败
 */
static int search_driver(int driver_id, std::map<int, platform::driver_dummy::driver_ptr>::iterator *item_iter) {
  auto iter = get_driver_map().find(driver_id);
  if (iter == get_driver_map().end() || !(iter->second)) {
    auto drv_str = devmgr_query_driver(driver_id);
    if (!drv_str) {
      return eno::ENO_NOTEXIST;
    }
    auto drv_ptr = new platform::driver_dummy;
    if (!drv_ptr) {
      return eno::ENO_NOMEM;
    }
    drv_ptr->from_json_str(drv_str);
    free_devmgr_string(drv_str);
    get_driver_map()[driver_id] = platform::driver_dummy::driver_ptr(drv_ptr);
    iter = get_driver_map().find(driver_id);
  }
  *item_iter = iter;
  return eno::ENO_OK;
}

/**
 * @brief 查找合适的设备
 * 
 * @note 优先从驱动缓存集中查询，查找不到则调用devmgr api
 * @param device_id 设备id
 * @param item_iter  查找到的结果
 * @return int 0  成功
 *             !0 失败
 */
static int search_device(int device_id, std::map<int, platform::device::device_ptr>::iterator *item_iter) {
  auto iter = get_device_map().find(device_id);
  if (iter == get_device_map().end() || !(iter->second)) {
    // need query device manager
    auto dev_str = devmgr_query_device(device_id);
    if (!dev_str) {
      return eno::ENO_NOTEXIST;
    }
    auto dev_ptr = new platform::device();
    if (!dev_ptr) {
      return eno::ENO_NOMEM;
    }
    dev_ptr->from_json_str(dev_str);
    free_devmgr_string(dev_str);
    get_device_map()[device_id] = platform::device::device_ptr(dev_ptr);
    iter = get_device_map().find(device_id);
  }
  *item_iter = iter;
  return eno::ENO_OK;
}

/**
 * @brief 通过devmgr更新device相关信息
 * 
 * @param device_id 设备id
 * @return int 0 成功
 */
static int update_device_info(int device_id) {
  const char *dev_str = devmgr_query_device(device_id);
  if (!dev_str) return eno::ENO_NOTEXIST;
  auto iter = get_device_map().end();
  int res = search_device(device_id, &iter);
  if (res) return res;
  iter->second->from_json_str(dev_str);
  return eno::ENO_OK;
}

/**
 * @brief 通过设备id获取该设备已绑定的驱动id
 * 
 * @param device_id 设备id
 * @return int id
 */
static int get_driver_id(int device_id) {
  auto iter = get_device_map().end();
  int res = search_device(device_id, &iter);
  if (res) {
    return res;
  }
  return iter->second->get_bind_id();
}

/**
 * @brief 生成函数的hash id
 *
 * @param method 调用方法的名称(与注册时保持一致即可, 如'open', 'close'等)
 * @param driver_name 驱动名称，用于区分各个驱动
 * @return hash_id
 */
static auto gen_hash(const char *method, const char *driver_name) {
  platform::alter::string s;
  s.append(driver_name);
  s.append("-");
  s.append(method);
  return syscall::hash(s);
}

extern "C" {

int dev_bind(int device_id, int driver_id) {
  auto iter = get_driver_map().end();
  int res = search_driver(driver_id, &iter);
  if (res) return res;
  auto hash = gen_hash("bind", iter->second->get_name());
  msg msgbuf = msg_wrapper::package(hash, device_id);
  platform::rcall_client client(iter->second->get_ipc_remote_channel());
  return client._call(0, msgbuf.get(), msgbuf.size(), nullptr, nullptr);
}

int dev_unbind(int device_id) {
  int driver_id = get_driver_id(device_id);
  if (!driver_id) return eno::ENO_NOTEXIST;
  auto iter = get_driver_map().end();
  int res = search_driver(driver_id, &iter);
  if (res) return res;
  update_device_info(device_id);
  auto hash = gen_hash("unbind", iter->second->get_name());
  auto msgbuf = msg_wrapper::package(hash, device_id);
  platform::rcall_client client(iter->second->get_ipc_remote_channel());
  return client._call(0, msgbuf.get(), msgbuf.size(), nullptr, nullptr);
}

int dev_open(int device_id, int flags) {
  int driver_id = get_driver_id(device_id);
  auto iter = get_driver_map().end();
  int res = search_driver(driver_id, &iter);
  if (res) return res;
  auto hash = gen_hash("open", iter->second->get_name());
  auto msgbuf = msg_wrapper::package(hash, device_id, flags);
  platform::rcall_client client(iter->second->get_ipc_remote_channel());
  return client._call(0, msgbuf.get(), msgbuf.size(), nullptr, nullptr);
}

int dev_close(int device_id) {
  int driver_id = get_driver_id(device_id);
  auto iter = get_driver_map().end();
  int res = search_driver(driver_id, &iter);
  if (res) return res;
  auto hash = gen_hash("close", iter->second->get_name());
  auto msgbuf = msg_wrapper::package(hash, device_id, driver_id);
  platform::rcall_client client(iter->second->get_ipc_remote_channel());
  return client._call(0, msgbuf.get(), msgbuf.size(), nullptr, nullptr);
}

int dev_transfer(int device_id, const void *in, size_t in_len, void *out, size_t out_len) {
  int driver_id = get_driver_id(device_id);
  auto iter = get_driver_map().end();
  int res = search_driver(driver_id, &iter);
  if (res) return res;
  auto hash = gen_hash("transfer", iter->second->get_name());
  msg_ref refmsg(out, out_len);
  auto msgbuf = msg_wrapper::package(hash, device_id, in, in_len, &refmsg);
  platform::rcall_client client(iter->second->get_ipc_remote_channel());
  size_t sz = refmsg.size();
  return client._call(0, msgbuf.get(), msgbuf.size(), refmsg.get(), &sz);
}
int dev_write(int device_id, const void *in, size_t len) {
  int driver_id = get_driver_id(device_id);
  auto iter = get_driver_map().end();
  int res = search_driver(driver_id, &iter);
  if (res) return res;
  auto hash = gen_hash("write", iter->second->get_name());
  auto msgbuf = msg_wrapper::package(hash, device_id, in, len);
  platform::rcall_client client(iter->second->get_ipc_remote_channel());
  return client._call(0, msgbuf.get(), msgbuf.size(), nullptr, nullptr);
}
int dev_read(int device_id, void *out, size_t len) {
  int driver_id = get_driver_id(device_id);
  auto iter = get_driver_map().end();
  int res = search_driver(driver_id, &iter);
  if (res) return res;
  auto hash = gen_hash("read", iter->second->get_name());
  msg_ref refmsg(out, len);
  auto msgbuf = msg_wrapper::package(hash, device_id, &refmsg);
  platform::rcall_client client(iter->second->get_ipc_remote_channel());
  size_t sz = refmsg.size();
  return client._call(0, msgbuf.get(), msgbuf.size(), refmsg.get(), &sz);
}

int dev_ioctl(int device_id, int cmds, void *in_out, size_t in_len, size_t buffer_max_len) {
  int driver_id = get_driver_id(device_id);
  auto iter = get_driver_map().end();
  int res = search_driver(driver_id, &iter);
  if (res) return res;
  auto hash = gen_hash("ioctl", iter->second->get_name());
  msg_ref refmsg(in_out, buffer_max_len);
  auto msgbuf = msg_wrapper::package(hash, device_id, cmds, in_out, in_len, &refmsg);
  platform::rcall_client client(iter->second->get_ipc_remote_channel());
  size_t sz = refmsg.size();
  return client._call(0, msgbuf.get(), msgbuf.size(), refmsg.get(), &sz);
}
}
