/**
 * @file syscall-test.cxx
 * @author savent (savent_gate@outlook.com)
 * @brief
 * @version 0.1
 * @date 2020-11-26
 *
 * Copyright 2020 jrlc
 *
 */
#include <platform/bits.hxx>
#include <platform/debug.hxx>
#include <platform/entry.hxx>
#include <platform/syscall.hxx>

static int foo(int id, const char* str, size_t len) { return 1; }

using platform::bits;
using platform::debug;

static void test_cast_syscall_package_msg() {
  size_t sz;
  char v1[] = "foo";
  void* v2 = v1;
  size_t v3 = 4;
  int v4 = 1028;

  auto buf = platform::syscall::package_msg(0, v1);
  // cmd(size_t) | buffer_len(size_t) | v1(char*) | v2(size_t) = 12
  void* mem = (*buf).get();
  debug::assert((*buf).size() == 12);
  debug::assert(*(reinterpret_cast<platform::syscall::hash_id*>(mem)) == 0);
  debug::assert(*(reinterpret_cast<size_t*>(bits::shift_addr(mem, sizeof(size_t)))) == 4);
  debug::assert(!strcmp(
      reinterpret_cast<const char*>(bits::shift_addr(mem, sizeof(size_t) + sizeof(platform::syscall::hash_id))), v1));

  // as a smart pointer, buf can relase memory automatically
  buf = platform::syscall::package_msg(100, v2, v3);
  mem = (*buf).get();
  // cmd(size_t) | v3(size_t) | v2(void*)
  debug::assert((*buf).size() == 12);
  debug::assert(*(reinterpret_cast<platform::syscall::hash_id*>(mem)) == 100);
  debug::assert(*(reinterpret_cast<size_t*>(bits::shift_addr(mem, sizeof(size_t)))) == 4);
  debug::assert(!strcmp(
      reinterpret_cast<const char*>(bits::shift_addr(mem, sizeof(size_t) + sizeof(platform::syscall::hash_id))), v1));

  buf = platform::syscall::package_msg(120, v1, v4);
  // cmd(size_t) | strlen(size_t) | v1(char*) | v4(int)
  platform::debug::assert(16);

  buf = platform::syscall::package_msg(120, v1, v2, v3, v4);
  // cmd(size_t) | strlen(size_t) | v1(char*) | v3(size_t) | v2(void*) | v4(int)
  platform::debug::assert(24);
}

static void test_case_syscall_echo() {
  auto inst = platform::syscall::get_instance();

  inst->add("test-case-echo", (platform::syscall::func_t)([](int v1, const char* v2, size_t v3) {
              debug::assert(v1 == 128);
              debug::assert(!strcmp(v2, "Hello,World\r\n"));
              debug::assert(v3 == 256);
              return 0;
            }));

  int a1 = 128;
  const char* a2 = "Hello,World\r\n";
  size_t a3 = 256;
  size_t sz;
  auto msg_buf = platform::syscall::package_msg(a1, a2, a3);

  inst->call(platform::syscall::hash("test-case-echo"), (*msg_buf).get(), sz);
}
extern "C" int test_case_syscall() {
  platform::entry::platform_init(nullptr);

  test_cast_syscall_package_msg();
  test_case_syscall_echo();

  // test syscall package msg's api
  auto syscall_hdl = platform::syscall::get_instance();

  syscall_hdl->add("FOO", foo);

  char foo_buf[] = {
      0x10, 0x00, 0x00, 0x00,  // int
      0x04, 0x00, 0x00, 0x00,  // const char *'s len
      'a',  'b',  'c',  0x00,  // const char * string
      0x04, 0x00, 0x00, 0x00,  // size_t len
  };
  syscall_hdl->call(platform::syscall::hash("FOO"), foo_buf, sizeof(foo_buf));
  return 0;
}

func_entry_level_untest(test_case_syscall);
