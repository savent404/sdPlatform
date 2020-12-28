/**
 * @file arm32-embedded-sys.cxx
 * @author savent (savent_gate@outlook.com)
 * @brief 针对裸机环境的部分libc, libstdc++实现
 * @version 0.1
 * @date 2020-11-19
 *
 * Copyright 2020 jrlc
 *
 */
#include <stddef.h>

#include <platform/debug.hxx>
#include <platform/os.hxx>

extern "C" void* heap_alloc(size_t n);
extern "C" void heap_free(void*);

#if 0
__attribute__((weak)) void* operator new(size_t n) { return heap_alloc(n); }
__attribute__((weak)) void operator delete(void* p) { heap_free(p); }
__attribute__((weak)) void operator delete(void* p, size_t n) { heap_free(p); }

__attribute__((weak)) void* operator new[](size_t n) { return heap_alloc(n); }
__attribute__((weak)) void operator delete[](void* p) { return heap_free(p); }
#else
__attribute__((weak)) void* operator new(size_t n) { return platform::ops::memory::mem_alloc(n); }
__attribute__((weak)) void operator delete(void* p) { platform::ops::memory::mem_free(p); }
__attribute__((weak)) void operator delete(void* p, size_t n) { platform::ops::memory::mem_free(p); }

__attribute__((weak)) void* operator new[](size_t n) { return platform::ops::memory::mem_alloc(n); }
__attribute__((weak)) void operator delete[](void* p) { return platform::ops::memory::mem_free(p); }
#endif

#include <stdlib.h>
#include <string.h>

extern "C" __attribute__((weak)) void abort() {
  platform::debug::assert(false);
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
  size_t sz = strlen(str) + 1;
  char* ptr = (char*)platform::ops::memory::mem_alloc(sz);  // NOLINT
  memcpy(ptr, str, sz);
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

extern "C" __attribute__((weak)) int rand(void) {
  static int holdrand = 0;
  return (((holdrand = holdrand * 214013L + 2531011L) >> 16) & 0x7fff);
}

extern "C" __attribute__((weak)) void* memset(void* buf, int val, size_t n) {
  char* t = static_cast<char*>(buf);
  while (n--) {
    *t++ = val;
  }
  return t;
}

extern "C" __attribute__((weak)) double strtod(const char* str, char** endptr) {
  double result = 0.0;
  char signedResult = '\0';
  char signedExponent = '\0';
  int decimals = 0;
  bool isExponent = false;
  bool hasExponent = false;
  bool hasResult = false;
  // exponent is logically int but is coded as double so that its eventual
  // overflow detection can be the same as for double result
  double exponent = 0;
  char c;

  for (; '\0' != (c = *str); ++str) {
    if ((c >= '0') && (c <= '9')) {
      int digit = c - '0';
      if (isExponent) {
        exponent = (10 * exponent) + digit;
        hasExponent = true;
      } else if (decimals == 0) {
        result = (10 * result) + digit;
        hasResult = true;
      } else {
        result += (double)digit / decimals;
        decimals *= 10;
      }
      continue;
    }
    if (c == '.') {
      if (!hasResult) break;     // don't allow leading '.'
      if (isExponent) break;     // don't allow decimal places in exponent
      if (decimals != 0) break;  // this is the 2nd time we've found a '.'

      decimals = 10;
      continue;
    }
    if ((c == '-') || (c == '+')) {
      if (isExponent) {
        if (signedExponent || (exponent != 0))
          break;
        else
          signedExponent = c;
      } else {
        if (signedResult || (result != 0))
          break;
        else
          signedResult = c;
      }
      continue;
    }
    if (c == 'E') {
      if (!hasResult) break;  // don't allow leading 'E'
      if (isExponent)
        break;
      else
        isExponent = true;
      continue;
    }
    break;  // unexpected character
  }
  if (isExponent && !hasExponent) {
    while (*str != 'E') --str;
  }
  if (!hasResult && signedResult) --str;
  if (endptr) *endptr = const_cast<char*>(str);
  for (; exponent != 0; --exponent) {
    if (signedExponent == '-')
      result /= 10;
    else
      result *= 10;
  }
  if (signedResult == '-' && result != 0) result = -result;
  return result;
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
}  // namespace )
