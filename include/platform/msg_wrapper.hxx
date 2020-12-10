#pragma once
#include <platform/msg.hxx>
#include <platform/debug.hxx>
#include <platform/syscall-details.hxx>

namespace platform {
struct msg_wrapper {
 public:
  template <typename... Args>
  static msg package(Args... args) {
    size_t sz = syscall_dtl::calculate_size<Args...>::get(args...);
    msg buff(sz);
    size_t sz1 = syscall_dtl::package_obj<Args...>::set(buff.get(), args...);
    debug::assert(sz == sz1);
    return std::move(buff);
  }
  template <typename... Args>
  static auto unpackage(const msg &buff) {
    std::tuple<Args...> ts;
    void *ptr = buff.buffer_;
    size_t sz = buff.sz_;
    debug::assert(syscall_dtl::do_parse<Args...>(ts, ptr, sz));
    return std::move(ts);
  }

 private:
};

}  // namespace platform
