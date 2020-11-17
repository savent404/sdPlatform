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

#include <cJSON.h>
#include <string.h>
#include <functional>
#include <initializer_list>
#include <map>
#include <memory>
#include <string>
#include <type_traits>
#include <utility>
#include <variant>

namespace platform {

/**
 * @brief parameters inherit from std::variant
 * @warning in embedded env no-expection means no check for variant's type
 */
struct parameters {
 public:
  using value_type = std::variant<bool, double, const char*>;
  using initial_item_type = std::pair<const char*, value_type>;
  using initial_list = std::initializer_list<initial_item_type>;
  using value_ref = parameters&;
  using value_ptr = std::unique_ptr<parameters>;

  explicit parameters(initial_list list) {
    for (auto& item : list) {
      maps_[std::string(item.first)] = item.second;
    }
  }
  bool set(const std::string& key, value_type value) {
    maps_[key] = value;
    return true;
  }
  bool set(const char* key, value_type value) {
    maps_[std::string(key)] = value;
    return true;
  }
  bool has(const std::string& key) {
    if (maps_.find(key) != maps_.end()) return true;
    return false;
  }
  bool has(const char* key) {
    if (maps_.find(std::string(key)) != maps_.end()) return true;
    return false;
  }
  value_type get(const std::string& key) { return maps_[key]; }
  value_type get(const char* key) { return maps_[std::string(key)]; }

  cJSON* toJson() const { return nullptr; }

  void fromJson(cJSON const* root) {
    if (!cJSON_IsObject(root) || !root->string || strcmp(root->string, "config")) return;
    recurse_parse(root->child);
  }

 private:
  void recurse_parse(cJSON* obj, std::string prefix = "") {
    while (obj) {
      const char* key = obj->string;
      switch (obj->type & 0xFF) {
        case cJSON_True:
        case cJSON_False:
          set(key, cJSON_IsTrue(obj) ? true : false);
          break;
        case cJSON_Number:
          set(key, cJSON_GetNumberValue(obj));
          break;
        case cJSON_String:
          set(key, strdup(cJSON_GetStringValue(obj)));
          break;
        case cJSON_Object:
          std::string new_prefix = prefix + "/";
          recurse_parse(obj->child, new_prefix);
          break;
      }
      obj = obj->next;
    }
  }
  std::map<std::string, value_type> maps_;
};
}  // namespace platform
