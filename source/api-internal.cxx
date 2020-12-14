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
#include <platform/debug.hxx>
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
 * @brief 提供查询/修改dev/drv信息的方法
 * @note 可缓存相关dev&drv信息用于减少调用devmgr的次数
 * 
 */
struct internal_query {
 public:
  using debug = platform::debug;
  using device_map_type = platform::alter::map<int, platform::device::device_ptr>;
  using device_map_iter = device_map_type::iterator;
  using driver_map_type = platform::alter::map<int, platform::driver_dummy::driver_ptr>;
  using driver_map_iter = driver_map_type::iterator;

 private:
  driver_map_type driver_map;
  device_map_type device_map;

 public:
  static internal_query &get_instance() {
    static internal_query *instance;
    if (!instance) instance = new internal_query;
    debug::assert(instance != nullptr);
    return *instance;
  }
  /**
   * @brief 获取驱动的缓存集
   *
   * @note 使用缓存的意义在于每次系统调用到来不需要通过devmgr api查询相关信息
   * @return std::map<int, platform::driver_dummy::driver_ptr> &
   */
  static driver_map_type &get_driver_map() { return get_instance().driver_map; }
  /**
   * @brief 获取设备的缓存集
   *
   * @note 使用缓存的意义在于每次系统调用到来不需要通过devmgr api查询相关信息
   * @return std::map<int, platform::device::device_ptr> &
   */
  static device_map_type &get_device_map() { return get_instance().device_map; }
  /**
   * @brief 查找合适的驱动
   *
   * @note 优先从驱动缓存集中查询，查找不到则调用devmgr api
   * @param driver_id 驱动的id
   * @param item_iter 查找到的结果
   * @return int 0  成功
   *             !0 失败
   */
  static int search_driver(int drv_id, driver_map_iter *item_iter) {
    auto iter = get_driver_map().find(drv_id);
    if (iter == get_driver_map().end() || !(iter->second)) {
      // 未从缓存中找到该驱动
      // 向devmgr查询该驱动信息
      auto drv_str = devmgr_query_driver(drv_id);
      if (!drv_str) {
        return eno::ENO_NOTEXIST;
      }
      // 构造dummy driver并向其填充信息
      auto drv_ptr = new platform::driver_dummy;
      if (!drv_ptr) {
        return eno::ENO_NOMEM;
      }
      drv_ptr->from_json_str(drv_str);
      free_devmgr_string(drv_str);
      get_driver_map()[drv_id] = platform::driver_dummy::driver_ptr(drv_ptr);
      iter = get_driver_map().find(drv_id);
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
  static int search_device(int device_id, device_map_iter *item_iter) {
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
};

extern "C" {

int dev_bind(int device_id, int driver_id) {
  internal_query::driver_map_iter iter;
  int res = internal_query::search_driver(driver_id, &iter);
  if (res) return res;
  return iter->second->dev_bind(device_id, driver_id);
}

int dev_unbind(int device_id) {
  int driver_id = internal_query::get_driver_id(device_id);
  if (!driver_id) return eno::ENO_NOTEXIST;
  internal_query::driver_map_iter iter;
  int res = internal_query::search_driver(driver_id, &iter);
  if (res) return res;
  // internal_query::update_device_info(device_id);
  return iter->second->dev_unbind(device_id);
}

int dev_open(int device_id, int flags) {
  int driver_id = internal_query::get_driver_id(device_id);
  internal_query::driver_map_iter iter;
  int res = internal_query::search_driver(driver_id, &iter);
  if (res) return res;
  return iter->second->dev_open(device_id, flags);
}

int dev_close(int device_id) {
  int driver_id = internal_query::get_driver_id(device_id);
  internal_query::driver_map_iter iter;
  int res = internal_query::search_driver(driver_id, &iter);
  if (res) return res;
  return iter->second->dev_close(device_id);
}

int dev_transfer(int device_id, const void *in, size_t in_len, void *out, size_t out_len) {
  int driver_id = internal_query::get_driver_id(device_id);
  internal_query::driver_map_iter iter;
  int res = internal_query::search_driver(driver_id, &iter);
  if (res) return res;
  return iter->second->dev_transfer(device_id, in, in_len, out, out_len);
}
int dev_write(int device_id, const void *in, size_t len) {
  int driver_id = internal_query::get_driver_id(device_id);
  internal_query::driver_map_iter iter;
  int res = internal_query::search_driver(driver_id, &iter);
  if (res) return res;
  return iter->second->dev_write(device_id, in, len);
}
int dev_read(int device_id, void *out, size_t len) {
  int driver_id = internal_query::get_driver_id(device_id);
  internal_query::driver_map_iter iter;
  int res = internal_query::search_driver(driver_id, &iter);
  if (res) return res;
  return iter->second->dev_read(device_id, out, len);
}

int dev_ioctl(int device_id, int cmds, void *in_out, size_t in_len, size_t buffer_max_len) {
  int driver_id = internal_query::get_driver_id(device_id);
  internal_query::driver_map_iter iter;
  int res = internal_query::search_driver(driver_id, &iter);
  if (res) return res;
  return iter->second->dev_ioctl(device_id, cmds, in_out, in_len, buffer_max_len);
}
}
