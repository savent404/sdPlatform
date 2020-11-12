cmake_minimum_required(VERSION 3.10)
project(driver_platform)

include_directories(include extern/cJSON)

add_library(cJSON extern/cJSON/cJSON.c extern/cJSON/cJSON_Utils.c)
link_libraries(cJSON)

add_library(sdPlatform-int SHARED source/driver-int.c)
target_link_libraries(sdPlatform-int cJSON)
install(TARGETS sdPlatform-int LIBRARY DESTINATION lib)

add_library(sdPlatform-ext source/driver-ext.c)
target_link_libraries(sdPlatform-ext cJSON)
install(TARGETS sdPlatform-ext LIBRARY DESTINATION lib)

file(GLOB SD_PLATFORM_PUB_INC "include/*.h")
install(FILES extern/cJSON/cJSON.h ${SD_PLATFORM_PUB_INC} DESTINATION include)

include(test.cmake)