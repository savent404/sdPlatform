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
#include <osal.h>
#include <rpc_api.h>
#include <loop.h>

#include <cstddef>
#include <tuple>

// clang-format off
#include <consthash/all.hxx>
#include <platform/bits.hxx>
#include <platform/syscall.hxx>
#include <platform/msg.hxx>
#include <platform/debug.hxx>
// clang-format on

// 回调函数定义
extern "C" s32 syscall_ipc_handler_cb(mx_channel_t* ch, void* data, u32 len) {
  int syscall_id = *(reinterpret_cast<int*>(data));

  auto syscall = platform::syscall::get_instance();
  data = platform::bits::shift_addr(data, sizeof(syscall_id));
  len -= sizeof(syscall_id);
  platform::msg reply_buf;
  int res = syscall->call(syscall_id, data, len, &reply_buf);
  if (reply_buf.size()) {
    rpc_reply_data(ch, res, reinterpret_cast<u8*>(reply_buf.get()), reply_buf.size());
  } else {
    rpc_reply(ch, res);
  }
  return 0;
}

static loop_t *loop_;
extern "C" __attribute__((noreturn)) void* syscall_ipc_handler(void*) {
  loop_ = loop_new();
  auto instance = platform::syscall::get_instance();

  platform::syscall::ipc_desc des;
  des.ch = loop_get_remote_channel(loop_get_master(loop_));
  instance->set_local_ipc(des);

  // ch = loop_get_remote_channel(loop_get_master(loop_));
  platform::debug::assert(loop_);
  platform::debug::assert(!loop_msg_register(loop_get_master(loop_), 0, syscall_ipc_handler_cb, 0));
  loop_run(loop_);
  return NULL;
}

namespace platform {

// flag this syscall has been forbiden
static int dummy_syscall(void) { return eno::ENO_NOSYSCALL; }

template <typename... Args>
int syscall::call_p(hash_id id, Args &... args) const {
  func_t f = find(id);
  return syscall_dtl::call_fn_from_param(f, args...);
}

int syscall::call(hash_id id, void *buf, size_t len, msg* out) const {
  func_t f = find(id);
  return syscall_dtl::call_fn_from_buffer(f, buf, len, out);
}

int syscall::call(void *buf, size_t len, msg* out) const {
  hash_id id = *reinterpret_cast<hash_id *>(buf);
  func_t f = find(id);
  buf = bits::shift_addr(buf, sizeof(id));
  return syscall_dtl::call_fn_from_buffer(f, buf, len, out);
}

syscall *syscall::get_instance() {
  static syscall *c = nullptr;
  if (!c) c = new syscall;
  return c;
}

syscall::ipc_desc syscall::get_local_ipc() {
  while (!local_ipc_.ch) {
    os.os_msleep(100);
  }
  return local_ipc_;
}
void syscall::set_local_ipc(ipc_desc param) { local_ipc_ = param; }
syscall::ipc_desc syscall::get_devmgr_ipc() { return devmgr_ipc_; }
void syscall::set_devmgr_ipc(ipc_desc param) { devmgr_ipc_ = param;}

bool syscall::add(string func_name, func_t func) { return add(hash(func_name), func); }
bool syscall::del(string func_name) { return del(hash(func_name)); }
bool syscall::add(hash_id func_id, func_t func) {
  kv_[func_id] = func;
  return true;
}
bool syscall::del(hash_id func_id) {
  kv_[func_id] = dummy_syscall;
  return true;
}
syscall::func_t syscall::find(hash_id id) const {
  auto iter = kv_.find(id);
  return iter == kv_.end() ? dummy_syscall : iter->second;
}

syscall::_msg_buf_t::_msg_buf_t() : buffer(nullptr), sz_(0) {}
syscall::_msg_buf_t::_msg_buf_t(_msg_buf_t &&other) : buffer(std::move(other.buffer)), sz_(other.sz_) {}
syscall::_msg_buf_t::~_msg_buf_t() { free_buffer(); }

void *syscall::_msg_buf_t::get() { return buffer.get(); }
size_t syscall::_msg_buf_t::size() { return sz_; }
void syscall::_msg_buf_t::set(char *ptr, size_t sz) {
  free_buffer();
  buffer = std::unique_ptr<char[]>(ptr);
  sz_ = sz;
}
void syscall::_msg_buf_t::free_buffer() { buffer = nullptr; }

bool syscall::init() {
  os.os_thread_create(syscall_ipc_handler, nullptr);
  return true;
}
bool syscall::deinit() {
  return false;
}

syscall::ipc_desc::ipc_desc() : ch(nullptr) {}
cJSON* syscall::ipc_desc::to_json() const {
  int pointer = reinterpret_cast<int>(ch);
  auto root = cJSON_CreateObject();
  cJSON_AddNumberToObject(root, "pointer", pointer);
  return root;
}
void syscall::ipc_desc::from_json(cJSON* obj) {
  if (cJSON_HasObjectItem(obj, "pointer")) {
    int pointer = cJSON_GetNumberValue(cJSON_GetObjectItem(obj, "pointer"));
    ch = reinterpret_cast<decltype(ch)>(pointer);
  }
}

}  // namespace platform
