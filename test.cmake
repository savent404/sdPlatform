cmake_minimum_required(VERSION 3.10)
project(driver_platform)

include_directories(include)

enable_testing()

add_executable(dlist-test tests/dlist-test.c)
install(TARGETS dlist-test RUNTIME DESTINATION bin/test)
ADD_TEST(NAME test_dlist_init COMMAND dlist-test)

add_executable(gpio-test tests/gpio-test.c)
ADD_TEST(NAME gpio-test COMMAND gpio-test)
install(TARGETS gpio-test RUNTIME DESTINATION bin/test)