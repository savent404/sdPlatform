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
  strcpy(ptr, str);                                // NOLINT
  return ptr;
}

extern "C" __attribute__((weak)) void* memmove(void* dest, const void* src, size_t n) {
  char* _d = (char*)dest;  // NOLINT
  char* _s = (char*)src;   // NOLINT
  while (n--) {
    *_d++ = *_s++;
  }
  return dest;
}

#if __cplusplus < 201103L
#error "placeholders.cc must be compiled with -std=gnu++0x"
#endif

#include <functional>

namespace std _GLIBCXX_VISIBILITY(default) {
  _GLIBCXX_BEGIN_NAMESPACE_VERSION

  namespace placeholders {
  extern const _Placeholder<1> _1{};
  extern const _Placeholder<2> _2{};
  extern const _Placeholder<3> _3{};
  extern const _Placeholder<4> _4{};
  extern const _Placeholder<5> _5{};
  extern const _Placeholder<6> _6{};
  extern const _Placeholder<7> _7{};
  extern const _Placeholder<8> _8{};
  extern const _Placeholder<9> _9{};
  extern const _Placeholder<10> _10{};
  extern const _Placeholder<11> _11{};
  extern const _Placeholder<12> _12{};
  extern const _Placeholder<13> _13{};
  extern const _Placeholder<14> _14{};
  extern const _Placeholder<15> _15{};
  extern const _Placeholder<16> _16{};
  extern const _Placeholder<17> _17{};
  extern const _Placeholder<18> _18{};
  extern const _Placeholder<19> _19{};
  extern const _Placeholder<20> _20{};
  extern const _Placeholder<21> _21{};
  extern const _Placeholder<22> _22{};
  extern const _Placeholder<23> _23{};
  extern const _Placeholder<24> _24{};
  extern const _Placeholder<25> _25{};
  extern const _Placeholder<26> _26{};
  extern const _Placeholder<27> _27{};
  extern const _Placeholder<28> _28{};
  extern const _Placeholder<29> _29{};
  }  // namespace placeholders

  _GLIBCXX_END_NAMESPACE_VERSION
}  // namespace std_GLIBCXX_VISIBILITY(default)
