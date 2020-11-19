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
__attribute__((weak)) void  operator delete[](void* p) { return heap_free(p); }

#include <stdlib.h>

extern "C" __attribute__((weak)) void* heap_alloc(size_t n) { return nullptr; }
extern "C" __attribute__((weak)) void heap_free(void* p) {  }
extern "C" __attribute__((weak)) void abort() { while (1) {} }
