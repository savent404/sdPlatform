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

template <typename T, typename... Args>
using param_t = std::tuple<T, Args...>;

/**
 * @brief 将指针后移
 *
 * @param pointer 指针
 * @param offset  位移大小(字节为单位)
 */
// static void __attribute__((used)) shift_buffer_point(const void*& pointer, size_t offset) {  // NOLINT
//   pointer = bits::shift_addr<const void*>(pointer, offset);
// }
static void shift_buffer_point(void*& pointer, size_t offset) {  // NOLINT
  pointer = bits::shift_addr<void*>(pointer, offset);
}

/**
 * @brief 从buffer中获取一个数据
 *
 * @tparam T 数据类型
 * @param[out] t 输出的数据
 * @param[in/out] buf
 * @param[in/out] len
 * @note 该函数仅能解析基础数据类型，复杂类型需要特例化该函数
 * @note 该函数会位移buff指针指向下一个数据起始位置
 * @note 该函数会减去*len的大小，指示buf剩余字节长度
 * @return true 获取成功
 * @return false 获取失败
 */
template <typename T>
bool parse_param(T& t, void*& buf, size_t& len) {  // NOLINT
  if (len < sizeof(t)) return false;
  t = *reinterpret_cast<T*>(buf);
  len -= sizeof(t);
  shift_buffer_point(buf, sizeof(t));
  return true;
}

/**
 * @brief convert 'const char*'
 *
 * @tparam
 * @param t
 * @param buf
 * @param len
 * @return true
 * @return false
 */
template <>
bool parse_param(const char*& t, void*& buf, size_t& len) {  // NOLINT
  // capture size from head
  size_t str_len = 0;
  parse_param(str_len, buf, len);

  if (len < str_len) return false;
  t = reinterpret_cast<const char*>(buf);
  len -= str_len;
  shift_buffer_point(buf, str_len);
  return true;
}

template <>
bool parse_param(void*& t, void*& buf, size_t& len) {  // NOLINT
  // capture size from head
  size_t size_len = 0;
  parse_param(size_len, buf, len);

  if (len < size_len) return false;
  t = reinterpret_cast<void*>(buf);
  len -= size_len;
  shift_buffer_point(buf, size_len);
  return true;
}

template <typename T>
bool do_parse(param_t<T>& ts, void*& buf, size_t& len) {  // NOLINT
  T val;
  if (!parse_param<T>(val, buf, len)) return false;
  ts = std::tie(val);
  return true;
}
template <typename T, typename... Args>
bool do_parse(param_t<T, Args...>& ts, void*& buf, size_t& len) {  // NOLINT
  T val;
  if (!parse_param<T>(val, buf, len)) return false;
  std::tuple<Args...> t;
  if (!do_parse(t, buf, len)) return false;
  ts = std::tuple_cat(std::tie(val), t);
  return true;
}

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
