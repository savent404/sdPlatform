#include <osal.h>
#include <smempool.h>
#include <string.h>

#include <platform/debug.hxx>
#include <platform/os.hxx>

#ifndef __weak
#define __weak __attribute__((weak))
#endif

// NOTE(savent): platform 中未包含ok相关头文件，故提供弱函数通过编译，连接到os时会使用os提供的函数
extern "C" __weak void *heap_alloc(size_t n) {
  platform::debug::assert(false);
  return nullptr;
}
extern "C" __weak void heap_free(void *p) { platform::debug::assert(false); }

namespace platform::ops::memory {

void mem_init(void) {
  smem_config_t config = {};
  config.malloc = heap_alloc;
  config.free = heap_free;
  config.list_default_bs = 2048;
  config.slab_8B_default_bs = 1024;
  config.slab_16B_default_bs = 256;
  config.slab_32B_default_bs = 256;
  config.slab_64B_default_bs = 256;
  config.slab_128B_default_bs = 256;
  smem_init(&config);
  ;
}

void *mem_alloc(size_t sz) { return smem_alloc(sz); }
void *mem_realloc(void *src, size_t sz) {
  auto ptr = smem_alloc(sz);
  if (!ptr) return nullptr;
  memcpy(ptr, src, sz);
  return ptr;
}
void mem_free(void *ptr) { smem_free(ptr); }

}  // namespace platform::ops::memory

namespace platform::ops::thread {
struct thread_ctx {
  ipc_handle_t ipc_handle;
};
thread_t thread_create(thread_entry_t entry, void *args, thread_pri_t pri) {
  auto tctx = new thread_ctx;
  tctx->ipc_handle = os.os_thread_create(entry, args);
  return tctx;
}

void thread_sleep(int ms) { os.os_msleep(ms); }
void thread_exit(int code) { os.os_exit(code); }

}  // namespace platform::ops::thread
