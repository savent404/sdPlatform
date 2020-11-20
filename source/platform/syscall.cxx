/**
 * @file syscall.cxx
 * @author savent (savent_gate@outlook.com)
 * @brief
 * @version 0.1
 * @date 2020-11-20
 *
 * Copyright 2020 jrlc
 *
 */
#include <platform-types.h>
#include <tuple>

#include <platform/syscall.hxx>

namespace platform {

// flag this syscall has been forbiden
static int dummy_syscall(void) { return eno::ENO_NOSYSCALL; }

syscall::hash_id syscall::hash(const string& name) {
  auto s = std::_Hash_impl::hash(name.data(), name.length());
  return s;
}

template <typename... Args>
int syscall::call_p(hash_id id, Args &... args) const {
  func_t f = find(id);
  return syscall_dtl::call_fn_from_param(f, args...);
}
int syscall::call(hash_id id, void *buf, size_t len) const {
  func_t f = find(id);
  return syscall_dtl::call_fn_from_buffer(f, buf, len);
}

syscall &syscall::get_instance() {
  static syscall *c = nullptr;
  if (!c) c = new syscall;
  return *c;
}

bool syscall::add(const string& func_name, func_t func) { return add(hash(func_name), func); }
bool syscall::del(const string& func_name) { return del(hash(func_name)); }
bool syscall::add(hash_id func_id, func_t func) { kv_[func_id] = func; }
bool syscall::del(hash_id func_id) { kv_[func_id] = dummy_syscall; }
syscall::func_t syscall::find(hash_id id) const {
  auto iter = kv_.find(id);
  return iter == kv_.end() ? dummy_syscall : iter->second;
}

}  // namespace platform
