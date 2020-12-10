/**
 * @file syscall-details.cxx
 * @author savent (savent_gate@outlook.com)
 * @brief 驱动接口调用的细节
 * @note  根据接口注册的callback形参类型来具体处理
 * @version 0.1
 * @date 2020-11-20
 *
 * Copyright 2020 jrlc
 *
 */
#include <platform-types.h>
#include <cstring>

#include <platform/bits.hxx>
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

    // f -> std::function<int, const char*, size_t)
    case 3: {
      auto f = std::get<3>(func);
      param_t<int, const char*, size_t> ts;
      if (!do_parse(ts, buf, len)) break;
      res = f(std::get<0>(ts), std::get<1>(ts), std::get<2>(ts));
    } break;

    // f -> std::function<int(int, void*, size_t)
    case 4: {
      auto f = std::get<4>(func);
      param_t<int, void*, size_t> ts;
      if (!do_parse(ts, buf, len)) break;
      // if (!do_parse<int, void*, char*, size_t>(ts, buf, len)) break;
      res = f(std::get<0>(ts), std::get<1>(ts), std::get<2>(ts));
    } break;

    default:
      res = eno::ENO_NOSYSCALL;
  }
  return res;
}

}  // namespace platform::syscall_dtl
