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

#include <platform-types.h>

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

template <typename T>
inline int ioctl_helper_get_param(const T &val, void *buf, size_t *sz, size_t max_sz) {
  size_t p_sz = sizeof(T);
  if (max_sz < p_sz) return eno::ENO_NOMEM;
  *(reinterpret_cast<T *>(buf)) = val;
  *sz = p_sz;
  return eno::ENO_OK;
}
template <typename T>
inline int ioctl_helper_set_param(T *val, const void *buf, size_t sz) {
  if (sizeof(T) > sz) return eno::ENO_INVALID;
  *val = *(reinterpret_cast<const T *>(buf));
  return eno::ENO_OK;
}

}  // namespace tools
}  // namespace platform
