/**
 * @file syscall-details.cxx
 * @author savent (savent_gate@outlook.com)
 * @brief
 * @version 0.1
 * @date 2020-11-20
 *
 * Copyright 2020 jrlc
 *
 */
#include <platform-types.h>

#include <platform/syscall-details.hxx>

namespace platform::syscall_dtl {
int call_fn_from_buffer(func_t func, void* buf, size_t len) {
  // as default, any error as syscall error
  int res = eno::ENO_SYSCALL_ERR;
  switch (func.index()) {
    // f -> std::funcion<int(void)>
    case 0: {
      auto f = std::get<0>(func);
      res = f();
    } break;

    // f -> std::function<int(int)>
    case 1: {
      auto f = std::get<1>(func);
      param_t<int> ts;
      do_parse<int>(ts, buf, len);
      res = f(std::get<0>(ts));
    } break;

    // f -> std::function<int(int, int)>
    case 2: {
      auto f = std::get<2>(func);
      param_t<int, int> ts;
      if (!do_parse<int, int>(ts, buf, len)) break;
      res = f(std::get<0>(ts), std::get<1>(ts));
    } break;

    default:
      res = eno::ENO_NOSYSCALL;
  }
  return res;
}
}  // namespace platform::syscall_dtl
