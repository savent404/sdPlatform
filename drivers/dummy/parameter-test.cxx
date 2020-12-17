/**
 * @file parameter-test.cxx
 * @author savent (savent_gate@outlook.com)
 * @brief
 * @version 0.1
 * @date 2020-11-26
 *
 * Copyright 2020 jrlc
 *
 */
#include <platform/entry.hxx>
#include <platform/parameter.hxx>
#include <platform/entry.hxx>

extern "C" int test_case_parameter() {

  platform::entry::platform_init(nullptr);
  platform::parameters param({
      {"config/a", "foo"},
      {"config/b", "boo"},
      {"name", "ffff"},
      {"compat", "fdfdfd"},
      {"config/base", 0x400000},
      {"config/range", 0x1000},
  }),
      param1({});

  auto str = param.to_json_str();
  param1.from_json_str(str);
  auto str1 = param1.to_json_str();
  platform::cJSON_free((void *)str);
  platform::cJSON_free((void *)str1);
  std::get<const char *>(param.get("config/a"));

  return 0;
}

// func_entry_level_untest(test_case_parameter);
