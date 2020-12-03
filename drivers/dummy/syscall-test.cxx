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
#include <platform/entry.hxx>
#include <platform/syscall.hxx>

static int foo(int id, const char* str, size_t len) { return 1; }

extern "C" int test_case_syscall() {
  platform::entry::platform_init(nullptr);
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
