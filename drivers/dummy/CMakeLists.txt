cmake_minimum_required(VERSION 3.10)
project(dummy)

set(target dummy)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} src)
add_library(${target} ${src})
target_include_directories(${target} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
