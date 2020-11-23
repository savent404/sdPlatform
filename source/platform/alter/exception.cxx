/**
 * @file exception.cxx
 * @author savent (savent_gate@outlook.com)
 * @brief  implement for exception.hxx
 * @version 0.1
 * @date 2020-11-19
 *
 * Copyright 2020 jrlc
 *
 */
#include <platform/alter/exception.hxx>

#ifndef __PLATFORM_THROW_OR_ABORT
#define __PLATFORM_THROW_OR_ABORT
#endif

#ifndef __weak
#define __weak __attribute__((weak))
#endif

// namespace __gnu_cxx {
// int __snprintf_lite(char* __buf, size_t __bufsize, const char* __fmt, va_list __ap);
// }

namespace std {

// void __throw_bad_exception() { _GLIBCXX_THROW_OR_ABORT(bad_exception()); }
__weak void __throw_bad_exception() { platform::alter::throw_or_abort(); }

// void __throw_bad_alloc() { _GLIBCXX_THROW_OR_ABORT(bad_alloc()); }
__weak void __throw_bad_alloc() { platform::alter::throw_or_abort(); }

// void __throw_bad_cast() { _GLIBCXX_THROW_OR_ABORT(bad_cast()); }
__weak void __throw_bad_cast() { platform::alter::throw_or_abort(); }

// void __throw_bad_typeid() { _GLIBCXX_THROW_OR_ABORT(bad_typeid()); }
__weak void __throw_bad_typeid() { platform::alter::throw_or_abort(); }

// void __throw_logic_error(const char* __s __attribute__((unused))) { _GLIBCXX_THROW_OR_ABORT(logic_error(_(__s))); }
__weak void __throw_logic_error(const char* __s __attribute__((unused))) { platform::alter::throw_or_abort(); }

// void __throw_domain_error(const char* __s __attribute__((unused))) { _GLIBCXX_THROW_OR_ABORT(domain_error(_(__s))); }
__weak void __throw_domain_error(const char* __s __attribute__((unused))) { platform::alter::throw_or_abort(); }

__weak void __throw_invalid_argument(const char* __s __attribute__((unused))) {
  platform::alter::throw_or_abort();
}
//   void __throw_invalid_argument(const char* __s __attribute__((unused))) {
//     _GLIBCXX_THROW_OR_ABORT(invalid_argument(_(__s)));
//   }

// void __throw_length_error(const char* __s __attribute__((unused))) { _GLIBCXX_THROW_OR_ABORT(length_error(_(__s))); }
__weak void __throw_length_error(const char* __s __attribute__((unused))) { platform::alter::throw_or_abort(); }

// void __throw_out_of_range(const char* __s __attribute__((unused))) { _GLIBCXX_THROW_OR_ABORT(out_of_range(_(__s))); }
__weak void __throw_out_of_range(const char* __s __attribute__((unused))) { platform::alter::throw_or_abort(); }

__weak void __throw_bad_function_call() { platform::alter::throw_or_abort(); }
extern "C" __weak void __cxa_pure_virtual() { platform::alter::throw_or_abort(); }

__weak void __throw_out_of_range_fmt(const char* __fmt, ...) { platform::alter::throw_or_abort(); }
//   void __throw_out_of_range_fmt(const char* __fmt, ...) {
//     const size_t __len = __builtin_strlen(__fmt);
//     // We expect at most 2 numbers, and 1 short string. The additional
//     // 512 bytes should provide more than enough space for expansion.
//     const size_t __alloca_size = __len + 512;
//     char* const __s = static_cast<char*>(__builtin_alloca(__alloca_size));
//     va_list __ap;

//     va_start(__ap, __fmt);
//     __gnu_cxx::__snprintf_lite(__s, __alloca_size, __fmt, __ap);
//     _GLIBCXX_THROW_OR_ABORT(out_of_range(_(__s)));
//     va_end(__ap);  // Not reached.
//   }

__weak void __throw_runtime_error(const char* __s __attribute__((unused))) { platform::alter::throw_or_abort(); }
//   void __throw_runtime_error(const char* __s __attribute__((unused))) {
//     _GLIBCXX_THROW_OR_ABORT(runtime_error(_(__s)));
//   }

// void __throw_range_error(const char* __s __attribute__((unused))) { _GLIBCXX_THROW_OR_ABORT(range_error(_(__s))); }
__weak void __throw_range_error(const char* __s __attribute__((unused))) { platform::alter::throw_or_abort(); }

__weak void __throw_overflow_error(const char* __s __attribute__((unused))) { platform::alter::throw_or_abort(); }
//   void __throw_overflow_error(const char* __s __attribute__((unused))) {
//     _GLIBCXX_THROW_OR_ABORT(overflow_error(_(__s)));
//   }

__weak void __throw_underflow_error(const char* __s __attribute__((unused))) {
  platform::alter::throw_or_abort();
}
//   void __throw_underflow_error(const char* __s __attribute__((unused))) {
//     _GLIBCXX_THROW_OR_ABORT(underflow_error(_(__s)));
//   }

// __attribute__((weak)) void __throw_bad_variant_access(char const *) { platform::alter::do_nothing(); }
}  // namespace std
