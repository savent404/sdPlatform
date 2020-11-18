cmake_minimum_required(VERSION 3.10)
project(driver_platform)

set(CMAKE_CXX_STANDARD 17)

include_directories(include extern/cJSON ${CMAKE_INCLUDE_PATH})
link_directories(${CMAKE_LIBRARY_PATH})

add_library(cJSON extern/cJSON/cJSON.c extern/cJSON/cJSON_Utils.c)
link_libraries(cJSON)

aux_source_directory(source user_source)
aux_source_directory(source/internal platform_source)
add_library(platform-user ${user_source} ${platform_source})
target_link_libraries(platform-user)

add_executable(cxx-test tests/cxx-test.cc)
target_link_libraries(cxx-test cJSON platform-user)
