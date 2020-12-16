cmake_minimum_required(VERSION 3.14)
project(SDPlatform)

set(CMAKE_CXX_STANDARD_REQUIRED 17)
set(sdp_src "")

option(USE_TEST "add unit test code" ON)
option(EXT_DRIVER "path to locate external driver dir" OFF)

if(USE_TEST)
    add_definitions("-D__USE_TEST__=1")
else()
    add_definitions("-D__USE_TEST__=0")
endif(USE_TEST)

if (${CMAKE_SYSTEM_PROCESSOR} STREQUAL arm)
    set(sdp_src "${sd_src};source/arm32-embedded-sys.cxx")
    add_definitions("-D__PLATFORM_EMBEDDED__=1")
else()
    add_definitions("-D__PLATFORM_EMBEDDED__=0")
endif()

aux_source_directory(source source_src)
aux_source_directory(source/platform platform_src)
aux_source_directory(source/platform/drivers drivers_src)
aux_source_directory(source/platform/alter alter_source)
add_subdirectory(extern/smempool)

set(sdp_src "${sd_src};${source_src};${platform_src};${drivers_src};${alter_source}")

add_library(sdp-user ${sdp_src} $<TARGET_OBJECTS:sMemPool>)
target_include_directories(sdp-user
    PUBLIC
        include
        extern/smempool/include
        extern/consthash/include
    PRIVATE
        ${CMAKE_INSTALL_PREFIX}/include
)

include(cmake/add-driver.cmake)
include_directories(include external/consthash/include external/smempool/include ${CMAKE_INSTALL_PREFIX}/include)
link_libraries(sdp-user)

ADD_DRIVERS(${CMAKE_CURRENT_SOURCE_DIR}/drivers)
if (EXT_DRIVER)
    ADD_DRIVERS(${EXT_DRIVER})
endif(EXT_DRIVER)

install (TARGETS sdp-user
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include)
install (FILES
    "include/platform.h"
    "include/platform-types.h"
    DESTINATION include)

set(search_dir ${CMAKE_BINARY_DIR}/drivers)
set(out_dir ${CMAKE_INSTALL_PREFIX}/obj)
add_custom_target(install_drivers
    COMMENT "installing drivers..."
    COMMAND bash extract-lib.sh ${search_dir} ${out_dir}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/cmake)