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
  ENO_NOSYSCALL = -5,
  ENO_SYSCALL_ERR = -6,
  ENO_NOMEM = -7,
  ENO_NOTIMPL = -8,
};

enum fflag {
  FF_READ = 0x01,
  FF_WRITE = 0x02,
  FF_POLL = 0x04,  // use no-block as default
};
/**
 * @}
 */
#ifdef __cplusplus
}
#endif
