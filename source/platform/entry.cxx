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
#include <rpc_api.h>
#include <smempool.h>

#include <platform/bits.hxx>
#include <platform/cJSON.hxx>
#include <platform/debug.hxx>
#include <platform/entry.hxx>
#include <platform/syscall.hxx>
#include <platform/os.hxx>

namespace platform {

int entry::platform_init(void* ipc_ch) {
  static bool inited = false;
  bool firstInit = !inited;

  if (!inited) {
    platform::ops::memory::mem_init();
    platform::cJSON_Hooks hooks = {
        .malloc_fn = smem_alloc,
        .free_fn = smem_free,
    };
    platform::cJSON_InitHooks(&hooks);

    inited = true;
  }
  if (firstInit) {
    auto syscall = syscall::get_instance();
    syscall->init();
  }
  // update ipc desc every time
  if (ipc_ch) {
    syscall::ipc_desc desc;
    desc.ch = (mx_channel_t*)ipc_ch;  // NOLINT
    auto syscall = syscall::get_instance();
    syscall->set_devmgr_ipc(desc);
  }
  return 0;
}

}  // namespace platform
