#pragma once

#include <cstddef>

namespace platform {

namespace ops {

namespace memory {

void mem_init(void);
void *mem_alloc(size_t sz);
void *mem_realloc(void *src, size_t sz);
void mem_free(void *ptr);

}  // namespace memory

namespace thread {
enum thread_priority {
  thread_priority_low = 0,
  thread_priority_normal,
  thread_priority_high,
  thread_priority_realtime,
};

using thread_t = void *;
using thread_entry_t = void *(*)(void *);
using thread_pri_t = thread_priority;

thread_t thread_create(thread_entry_t entry, void *args, thread_pri_t pri = thread_priority_normal);
void thread_sleep(int ms);
void thread_exit(int code);
}  // namespace thread

}  // namespace ops

}  // namespace platform
