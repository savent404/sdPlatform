/**
 * @file utils.hxx
 * @author savent (savent_gate@outlook.com)
 * @brief
 * @version 0.1
 * @date 2020-11-17
 *
 * Copyright 2020 jrlc
 *
 */
#pragma once

#include <platform/cJSON.hxx>

namespace platform {
namespace tools {

template <typename iType, typename sType>
inline sType serialize(iType obj);

template <typename iType, typename sType>
inline iType deserialize(sType serail);

template <>
inline const char *serialize<cJSON *, const char *>(cJSON *obj) {
  return cJSON_Print(obj);
}

template <>
inline cJSON *deserialize<cJSON *, const char *>(const char *obj) {
  return cJSON_Parse(obj);
}

}  // namespace tools
}  // namespace platform
