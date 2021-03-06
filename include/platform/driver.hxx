/**
 * @file driver.hxx
 * @author savent (savent_gate@outlook.com)
 * @brief
 * @version 0.1
 * @date 2020-11-17
 *
 * Copyright 2020 jrlc
 *
 */
#pragma once

#include <platform-types.h>
#include <stddef.h>

#include <list>
#include <memory>
#include <platform/cJSON.hxx>
#include <platform/device.hxx>
#include <platform/msg.hxx>
#include <platform/parameter.hxx>
#include <platform/runtime.hxx>
#include <platform/utils.hxx>
#include <utility>
/**
 * @brief platform API
 * @defgroup platform_api
 * @{
 */
namespace platform {

/**
 * @brief platform driver api
 */
struct driver {
 public:
  using driver_id = int;
  using driver_ptr = std::unique_ptr<driver>;
  using driver_ref = driver&;
  using device_id = device::device_id;
  using device_ptr = device::device_ptr;
  using device_ref = device::device_ref;
  using device_kv = std::pair<device_id, device_ptr>;
  using device_kv_list = std::list<device_kv>;
  using parameters_ptr = parameters::value_ptr;
  using parameters_ref = parameters::value_ref;
  using runtime_ptr = runtime::value_ptr;
  using runtime_ref = runtime::value_ref;

  /**
   * @brief Construct a new driver object
   *
   * @param config_list 配置文件的初始化列表
   * @param runtime 默认为nullptr
   * @code
   * auto dri = driver({
   *  {"name", "demo-driver"},
   *  {"compat", "arm,demo-device"},
   * }, nullptr);
   * @endcode
   */
  explicit driver(parameters::initial_list config_list, runtime_ptr runtime);
  /**
   * @brief Construct a new driver object
   * @note 从json字符中解析参数并构造driver数据结构
   * @param json json字符串
   */
  explicit driver(const char* json);
  virtual ~driver();

  /**
   * @brief 初始化驱动
   *
   * @param argc
   * @param argv
   * @return int (enum eno)
   */
  int init(int argc, char** argv);
  /**
   * @brief 注销驱动
   *
   * @return int (enum eno)
   */
  int deinit();
  /**
   * @brief 将驱动和设备绑定
   *
   * @note 驱动根据device_id查询到设备相关信息，并尝试绑定并初始化设备
   * @param dev device_id
   * @return int (enum eno)
   */
  int bind(device_id dev);
  /**
   * @brief 取消驱动和设备的绑定
   *
   * @param dev  device_id
   * @return int (enum eno)
   */
  int unbind(device_id dev);
  /**
   * @brief 打开设备
   *
   * @param dev device_id
   * @param flags see enum fflags
   * @return int (enum eno)
   */
  int open(device_id dev, int flags);
  /**
   * @brief 关闭设备
   *
   * @param dev device_id
   * @return int (enum eno)
   */
  int close(device_id dev);
  /**
   * @brief 读写设备
   */
  int transfer(device_id dev, const void* in, size_t in_len, msg* out);
  /**
   * @brief 写设备
   *
   * @param dev device_id
   * @param[in] in 需要写入设备的数据指针
   * @param len 需要写入的字节数
   * @return int (enum eno)
   */
  int write(device_id dev, const void* in, size_t len);
  /**
   * @brief 读设备
   */
  int read(device_id dev, msg* out);
  /**
   * @brief 控制接口
   */
  int ioctl(device_id dev, int cmds, const void* in, size_t in_len, msg* out);

  /**
   * @brief 将数据转换为json对象
   *
   * @warning 返回的json对象应当使用 cJSON_free()来销毁
   * @return cJSON*
   */
  cJSON* to_json();
  /**
   * @brief 从json对象中更新数据
   *
   * @param obj cJSON*
   */
  void from_json(cJSON* obj);
  /**
   * @brief 将数据转换为json字符串
   *
   * @warning 返回的字符串数据应当使用 cJSON_free()销毁
   * @return const char*
   */
  const char* to_json_str();
  /**
   * @brief 从json字符串中更新数据
   *
   * @param json json字符串
   */
  void from_json_str(const char* json);
  /**
   * @brief 获取driver name字段
   *
   * @return const char*
   */
  const char* get_name();
  /**
   * @brief 获取driver compat字段
   *
   * @return const char*
   */
  const char* get_compat();
  /**
   * @brief 获取driver id
   *
   * @return driver_id
   */
  driver_id get_id();
  void set_id(driver_id id);
  // protected:
  /**
   * @brief devmanager's api
   * @defgroup driver_devmgr_api
   * @{
   */

  /**
   * @brief 向设备管理器注册驱动
   *
   * @return !0 返回注册的驱动id
   * @return 0 注册失败
   */
  driver_id devmgr_register();
  /**
   * @brief 主动通知设备管理更新驱动数据
   *
   * @note 将驱动的数据打包并上传给设备管理器
   */
  void devmgr_update();
  /**
   * @brief 向设备管理器查询该设备的信息
   *
   * @note 驱动在收到通知后调用该接口更新自身数据
   */
  void devmgr_query();
  /**
   * @brief 通知设备管理器注销设备
   *
   */
  void devmgr_delete();
  /**
   * @}
   */

 protected:
  virtual int init_(int argc, char** argv) = 0;
  virtual int deinit_() = 0;
  virtual int bind_(device_ref dev) = 0;
  virtual int unbind_(device_ref dev) = 0;
  virtual int open_(device_ref dev, int flags) = 0;
  virtual int close_(device_ref dev) = 0;
  virtual int transfer_(device_ref dev, const void* in, size_t in_len, void* out, size_t out_len) = 0;
  virtual int write_(device_ref dev, const void* in, size_t len) = 0;
  virtual int read_(device_ref dev, void* out, size_t len) = 0;
  virtual int ioctl_(device_ref dev, int cmds, const void* in, size_t in_len, void* out, size_t* out_len,
                     size_t buffer_max) = 0;
  virtual void to_json_(cJSON* obj) = 0;
  virtual void from_json_(cJSON* obj) = 0;
  virtual void register_internal_syscall_() = 0;

  parameters_ref get_config();
  runtime_ref get_runtime();

  template <typename T>
  static int ioctl_helper_get_param(const T& val, void* buf, size_t* sz, size_t max_sz);
  template <typename T>
  static int ioctl_helper_set_param(T& out, const void* buf, size_t sz);

 private:
  bool add_device(device_id id, device_ptr ptr);
  bool has_device(device_id id);
  bool del_device(device_id id);
  device_ptr get_device(device_id id);
  bool put_device(device_id, device_ptr ptr);

  device_ptr query_device(device_id id);
  bool update_device(device_id id, device_ref dev);

  void register_syscall();

 protected:
  parameters config_;
  runtime_ptr runtime_;

 private:
  device_kv_list device_list_;
};

bool _register_driver_hook_(driver* ptr);
}  // namespace platform
/**
 * @}
 */
