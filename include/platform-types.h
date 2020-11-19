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
  ENO_OK = 0,
  ENO_INVALID = -1,
  ENO_NOPERMIT = -2,
  ENO_BUSY = -3,
  ENO_NOTEXIST = -4,
};
/**
 * @}
 */
#ifdef __cplusplus
}
#endif
