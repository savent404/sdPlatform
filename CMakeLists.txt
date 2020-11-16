cmake_minimum_required(VERSION 3.10)
project(driver_platform)

include_directories(include extern/cJSON ${CMAKE_INCLUDE_PATH})
link_directories(${CMAKE_LIBRARY_PATH})

add_library(cJSON extern/cJSON/cJSON.c extern/cJSON/cJSON_Utils.c)
link_libraries(cJSON)

aux_source_directory(source/internal internal_source)
aux_source_directory(source/internal/drivers drivers_source)
add_library(sdPlatform-int STATIC source/driver-int.c ${internal_source} ${drivers_source})
target_link_libraries(sdPlatform-int cJSON)
install(TARGETS sdPlatform-int LIBRARY DESTINATION lib)

add_library(sdPlatform-ext source/driver-ext.c)
target_link_libraries(sdPlatform-ext SHARED cJSON)
install(TARGETS sdPlatform-ext LIBRARY DESTINATION lib)

file(GLOB SD_PLATFORM_PUB_INC "include/*.h")
install(FILES extern/cJSON/cJSON.h ${SD_PLATFORM_PUB_INC} DESTINATION include)

include(test.cmake)