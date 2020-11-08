cmake_minimum_required(VERSION 3.10)
project(driver_platform)

include_directories(include)

add_executable(foo foo.c)

include(test.cmake)