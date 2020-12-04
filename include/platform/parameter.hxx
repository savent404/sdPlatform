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
 * @brief duplicate a string
 *
 * @param src source string
 * @return char* copy
 */
extern "C" char* strdup(const char* src);

namespace platform {

/**
 * @brief parameters inherit from std::variant
 * @warning in embedded env no-expection means no check for variant's type
 */
struct parameters {
 public:
  using value_type = std::variant<bool, int, const char*>;
  using initial_item_type = std::pair<const char*, value_type>;
  using initial_list = std::initializer_list<initial_item_type>;
  using value_ref = parameters&;
  using value_ptr = std::unique_ptr<parameters>;
  using string = platform::alter::string;
  using kv_map = platform::alter::map<string, value_type>;

  explicit parameters(initial_list list) {
    for (auto& item : list) {
      maps_[string(item.first)] = item.second;
    }
  }
  bool set(const string& key, value_type value) {
    maps_[key] = value;
    return true;
  }
  bool set(const char* key, value_type value) {
    maps_[string(key)] = value;
    return true;
  }
  bool has(const string& key) {
    if (maps_.find(key) != maps_.end()) return true;
    return false;
  }
  bool has(const char* key) {
    if (maps_.find(string(key)) != maps_.end()) return true;
    return false;
  }

  template<typename T>
  T get(const string& key) {
    debug::assert(has(key));
    return std::get<T>(maps_[key]);
  }
  template<typename T>
  T get(const char *key) {
    debug::assert(has(key));
    return std::get<T>(maps_[string(key)]);
  }

  value_type get(const string& key) { return maps_[key]; }
  value_type get(const char* key) { return maps_[string(key)]; }

  cJSON* to_json() const {
    cJSON* root = cJSON_CreateObject();
    for (auto& item : maps_) {
      recurse_print(root, item.first, item.second);
    }
    return root;
  }
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
    while (obj) {
      // const char* key = obj->string;
      string key = prefix + obj->string;
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
