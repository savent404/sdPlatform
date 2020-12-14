/**
 * @file parameter.hxx
 * @author savent (savent_gate@outlook.com)
 * @brief
 * @version 0.1
 * @date 2020-11-17
 *
 * Copyright 2020 jrlc
 *
 */
#pragma once

#include <string.h>
#include <functional>
#include <initializer_list>
#include <memory>
#include <string>
#include <type_traits>
#include <utility>
#include <variant>

#include <platform/alter/map.hxx>
#include <platform/alter/string.hxx>
#include <platform/cJSON.hxx>
#include <platform/debug.hxx>

/**
 * @brief 返回字符串副本
 *
 * @param src 原字符串
 * @return char* 字符串副本
 */
extern "C" char* strdup(const char* src);

namespace platform {

/**
 * @brief 参数管理类
 * @note 参数类型继承自 std::variant
 * @warning 在裸机环境因缺少抛出 c异常的方法，所以不是类型安全的方法类。编程时需注意判断类型
 */
struct parameters {
 public:
  // 参数支持 bool, int, const char * 三种类型
  using value_type = std::variant<bool, int, const char*>;
  using initial_item_type = std::pair<const char*, value_type>;
  using initial_list = std::initializer_list<initial_item_type>;
  using value_ref = parameters&;
  using value_ptr = std::unique_ptr<parameters>;
  using string = platform::alter::string;
  using kv_map = platform::alter::map<string, value_type>;

  /**
   * @brief 构造函数-通过初始化列表
   * 
   * @param list 初始化列表
   */
  explicit parameters(initial_list list) {
    for (auto& item : list) {
      maps_[string(item.first)] = item.second;
    }
  }
  /**
   * @brief 设置变量
   * 
   * @param key 字符串
   * @param value 参数值
   * @return true 成功
   * @return false 失败
   */
  bool set(const string& key, value_type value) {
    maps_[key] = value;
    return true;
  }
  /**
   * @brief 设置变量
   * 
   * @param key 字符串
   * @param value 参数值
   * @return true 成功
   * @return false 失败
   */
  bool set(const char* key, value_type value) {
    maps_[string(key)] = value;
    return true;
  }
  /**
   * @brief 确认是否保存有该键(key)
   * 
   * @param key 字符串
   * @return true 确认存在
   * @return false 确认不存在
   */
  bool has(const string& key) {
    if (maps_.find(key) != maps_.end()) return true;
    return false;
  }
  /**
   * @brief 确认是否保存有该键(key)
   * 
   * @param key 字符串
   * @return true 确认存在
   * @return false 确认不存在
   */
  bool has(const char* key) {
    if (maps_.find(string(key)) != maps_.end()) return true;
    return false;
  }

  /**
   * @brief 获取参数
   * 
   * @tparam T bool, int 或 const char*
   * @param key 键
   * @note 已检查是否存在该键
   * @warning 类型检查不安全
   * @return T value_type
   */
  template<typename T>
  T get(const string& key) {
    debug::assert(has(key));
    return std::get<T>(maps_[key]);
  }
  /**
   * @brief 获取参数
   * 
   * @tparam T bool, int 或 const char*
   * @param key 键
   * @note 已检查是否存在该键
   * @warning 类型检查不安全
   * @return T value_type
   */
  template<typename T>
  T get(const char *key) {
    debug::assert(has(key));
    return std::get<T>(maps_[string(key)]);
  }

  /**
   * @brief 获取参数
   * 
   * @param key 键
   * @warning 未检查是否存在该键
   * @return value_type 
   */
  value_type get(const string& key) { return maps_[key]; }
  /**
   * @brief 获取参数
   * 
   * @param key 键
   * @warning 未检查是否存在该键
   * @return value_type 
   */
  value_type get(const char* key) { return maps_[string(key)]; }

  /**
   * @brief 将所有参数序列化为json对象
   * 
   * @note 结果不包含 'ipc', 'runtime'两个节点及其子节点
   * @return cJSON* 
   */
  cJSON* to_json() const {
    cJSON* root = cJSON_CreateObject();
    for (auto& item : maps_) {
      recurse_print(root, item.first, item.second);
    }
    return root;
  }
  /**
   * @brief 将所有参数序列化为json字符串
   * 
   * @note 结果不包含 'ipc', 'runtime'两个节点及其子节点
   * @return const char* 
   */
  const char * to_json_str() const {
    auto obj = to_json();
    auto res = cJSON_PrintUnformatted(obj);
    cJSON_free(obj);
    return res;
  }

  void from_json(cJSON const* root) {
    if (!cJSON_IsObject(root) || !root->child) return;
    recurse_parse(root->child);
  }

  void from_json_str(const char *str) {
    auto obj = cJSON_Parse(str);
    from_json(obj);
    cJSON_free(obj);
  }

 private:
  void recurse_print(cJSON* root, string key, const value_type& value) const {
    auto pos = key.find('/');
    if (string::npos == pos) {
      switch (value.index()) {
        case 0:  // bool
          cJSON_AddBoolToObject(root, key.c_str(), std::get<bool>(value));
          break;
        case 1:  // int
          cJSON_AddNumberToObject(root, key.c_str(), std::get<int>(value));
          break;
        case 2:  // const char *
          cJSON_AddStringToObject(root, key.c_str(), std::get<const char*>(value));
          break;
        default:
          break;
      }
    } else {
      string obj_name = key.substr(0, pos);
      cJSON* obj;
      if (!cJSON_HasObjectItem(root, obj_name.c_str())) {
        obj = cJSON_CreateObject();
        cJSON_AddItemToObject(root, obj_name.c_str(), obj);
      } else {
        obj = cJSON_GetObjectItem(root, obj_name.c_str());
      }
      string sub_name = key.substr(pos + 1);
      recurse_print(obj, sub_name, value);
    }
  }
  void recurse_parse(cJSON* obj, string prefix = "") {
    const char *black_list[] = {"runtime", "ipc"};
    while (obj) {
      // const char* key = obj->string;
      string key = prefix + obj->string;
      // match black list
      for (auto& item : black_list) {
        if (!strcmp(item, key.c_str())) return;
      }
      switch (obj->type & 0xFF) {
        case cJSON_True:
        case cJSON_False:
          set(key, cJSON_IsTrue(obj) ? true : false);
          break;
        case cJSON_Number:
          set(key, (int)cJSON_GetNumberValue(obj));
          break;
        case cJSON_String:
          set(key, strdup(cJSON_GetStringValue(obj)));
          break;
        case cJSON_Object:
          string new_prefix = prefix + key + "/";
          recurse_parse(obj->child, new_prefix);
          break;
      }
      obj = obj->next;
    }
  }
  kv_map maps_;
};
}  // namespace platform
