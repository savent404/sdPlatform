/**
 * @file platform-types.h
 * @author savent (savent_gate@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2020-11-17
 * 
 * Copyright 2020 jrlc
 * 
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif
/**
 * @brief user API
 * @defgroup user_api
 * @{
 */
enum eno {
  EOK = 0,
  EINVALID = -1,
  ENOPERMIT = -2,
  EBUSY = -3,
  ENOTEXIST = -4,
};
/**
 * @}
 */
#ifdef __cplusplus
}
#endif
