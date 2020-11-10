cmake_minimum_required(VERSION 3.10)
project(driver_platform)

include_directories(include extern/cJSON)

add_library(cJSON extern/cJSON/cJSON.c extern/cJSON/cJSON_Utils.c)
link_libraries(cJSON)


add_library(sdPlatform-int source/driver-int.c)
add_library(sdPlatform-ext source/driver-ext.c)

add_executable(foo foo.c)

include(test.cmake)