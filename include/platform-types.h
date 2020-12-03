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
  ENO_NOUSRMEM = -9,
};

enum fflag {
  FF_READ = 0x01,
  FF_WRITE = 0x02,
  FF_POLL = 0x04,  // use no-block as default
};

enum fcmds {
  /**
   * @brief uart cmds
   *
   */
  FC_GET_BAUD_RATE = 0x1010,
  FC_GET_PARITY_MODE,
  FC_GET_STOP_BIT,

  FC_SET_BAUD_RATE = 0x2010,
  FC_SET_PARITY_MODE,
  FC_SET_STOP_BIT,
  /**
   */
};

#define IS_QUERY_FCMDS(x) (x & 0x1000)
#define IS_CONFIG_FCMDS(x) (x & 0x8000)
/**
 * @}
 */
#ifdef __cplusplus
}
#endif
