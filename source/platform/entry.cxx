/**
 * @file entry.cxx
 * @author savent (savent_gate@outlook.com)
 * @brief
 * @version 0.1
 * @date 2020-12-03
 *
 * Copyright 2020 jrlc
 *
 */
#include <loop.h>
#include <memory.h>
#include <osal.h>
#include <rpc_api.h>
#include <smempool.h>

#include <platform/bits.hxx>
#include <platform/cJSON.hxx>
#include <platform/debug.hxx>
#include <platform/entry.hxx>
#include <platform/syscall.hxx>

#ifndef __weak
#define __weak __attribute__((weak))
#endif

// FIXME(savent): 电子科大不提供malloc/free标准函数
#define MALLOC_FN heap_alloc
#define FREE_FN heap_free

extern "C" void* malloc(size_t n);
extern "C" void free(void* p);
// NOTE(savent): platform 中未包含ok相关头文件，故提供弱函数通过编译，连接到os时会使用os提供的函数
extern "C" __weak void* heap_alloc(size_t n) { return nullptr; }
extern "C" __weak void heap_free(void* p) {
  while (1) {
  }
}

namespace platform {

entry::ipc_desc::ipc_desc() : ch(nullptr) {}
entry::ipc_desc::ipc_desc(const ipc_desc& other) : ch(other.ch) {}
cJSON* entry::ipc_desc::to_json() const {
  int pointer = reinterpret_cast<int>(ch);
  auto root = cJSON_CreateObject();
  cJSON_AddNumberToObject(root, "pointer", pointer);
  return root;
}
void entry::ipc_desc::from_json(cJSON* obj) {
  if (cJSON_HasObjectItem(obj, "pointer")) {
    int pointer = cJSON_GetNumberValue(cJSON_GetObjectItem(obj, "pointer"));
    ch = reinterpret_cast<decltype(ch)>(pointer);
  }
}
static entry::ipc_desc ipc_desc_;

extern "C" s32 driver_platform_ipc_handler_cb(mx_channel_t* ch, void* data, u32 len) {
  int syscall_id = *(reinterpret_cast<int*>(data));

  auto syscall = syscall::get_instance();
  data = bits::shift_addr(data, sizeof(syscall_id));
  len -= sizeof(syscall_id);
  msg reply_buf;
  int res = syscall->call(syscall_id, data, len, &reply_buf);
  if (reply_buf.size()) {
    rpc_reply_data(ch, res, reinterpret_cast<u8*>(reply_buf.get()), reply_buf.size());
  } else {
    rpc_reply(ch, res);
  }
  return 0;
}

extern "C" __attribute__((noreturn)) void* driver_platform_ipc_handler(void*) {
  auto loop = loop_new();
  ipc_desc_.ch = loop_get_remote_channel(loop_get_master(loop));
  debug::assert(loop);
  debug::assert(!loop_msg_register(loop_get_master(loop), 0, driver_platform_ipc_handler_cb, 0));
  loop_run(loop);
  return NULL;
}

const entry::ipc_desc entry::get_ipc_description() { return ipc_desc_; }

int entry::platform_init(void* ipc_ch) {
  static bool inited = false;

  // update ipc desc every time
  if (ipc_ch) {
    auto syscall = syscall::get_instance();
    syscall->set_ipc_description(ipc_ch);
  }

  if (inited) return 0;

  smem_config_t config;
  memset(&config, 0, sizeof(config));
  config.malloc = MALLOC_FN;
  config.free = FREE_FN;
  config.list_default_bs = 2048;
  config.slab_8B_default_bs = 1024;
  config.slab_16B_default_bs = 256;
  config.slab_32B_default_bs = 256;
  config.slab_64B_default_bs = 256;
  config.slab_128B_default_bs = 256;
  smem_init(&config);

  platform::cJSON_Hooks hooks = {
      .malloc_fn = smem_alloc,
      .free_fn = smem_free,
  };
  platform::cJSON_InitHooks(&hooks);

  os.os_thread_create(driver_platform_ipc_handler, ipc_ch);

  inited = true;
  return 0;
}

}  // namespace platform
