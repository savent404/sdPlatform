/**
 * @file arm32-embedded-sys.cxx
 * @author savent (savent_gate@outlook.com)
 * @brief for nonsys target
 * @version 0.1
 * @date 2020-11-19
 *
 * Copyright 2020 jrlc
 *
 */
#include <stddef.h>

extern "C" void* heap_alloc(size_t n);
extern "C" void heap_free(void*);

__attribute__((weak)) void* operator new(size_t n) { return heap_alloc(n); }
__attribute__((weak)) void operator delete(void* p) { heap_free(p); }
__attribute__((weak)) void operator delete(void* p, size_t n) { heap_free(p); }

__attribute__((weak)) void* operator new[](size_t n) { return heap_alloc(n); }
__attribute__((weak)) void operator delete[](void* p) { return heap_free(p); }

#include <stdlib.h>
#include <string.h>

extern "C" __attribute__((weak)) void* heap_alloc(size_t n) { return nullptr; }
extern "C" __attribute__((weak)) void heap_free(void* p) {}
extern "C" __attribute__((weak)) void abort() {
  while (1) {
  }
}
extern "C" __attribute__((weak)) size_t strlen(const char* str) {
  size_t n = 0;
  while (*str++) {
    n++;
  }
  return n;
}

extern "C" __attribute__((weak)) char* strcpy(char* dest, const char* src) {  // NOLINT
  char* _d = dest;
  const char* _s = src;
  while (*_s) {
    *_d++ = *_s++;
  }
  *_d = '\0';
  return _d;
}

extern "C" __attribute__((weak)) char* strdup(const char* str) {
  char* ptr = (char*)heap_alloc(strlen(str) + 1);  // NOLINT
  strcpy(ptr, str); // NOLINT
  return ptr;
}
