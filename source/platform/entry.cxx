#include <memory.h>
#include <smempool.h>

#include <platform/cJSON.hxx>
#include <platform/entry.hxx>

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
extern "C" __weak void heap_free(void* p) { while(1) {} }

namespace platform {

int entry::platform_init() {
  static bool inited = false;
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
  inited = true;
  return 0;
}

}  // namespace platform
