cmake_minimum_required(VERSION 3.10)
project(driver_platform)

set(CMAKE_CXX_STANDARD 17)

include_directories(include
                    extern/cJSON
                    extern/consthash/include
                    ${CMAKE_INCLUDE_PATH}
                    ${CMAKE_INSTALL_PREFIX}/include)
link_directories(${CMAKE_LIBRARY_PATH})

add_library(cJSON extern/cJSON/cJSON.c extern/cJSON/cJSON_Utils.c)
link_libraries(cJSON)

if (${CMAKE_SYSTEM_PROCESSOR} STREQUAL arm)
    set(sys_source "source/arm32-embedded-sys.cxx")
    add_definitions("-D__PLATFORM_EMBEDDED__=1")
else()
    add_definitions("-D__PLATFORM_EMBEDDED__=0")
endif()

aux_source_directory(source user_source)
set(user_source source/api-internal.cxx source/requirements.cxx)
aux_source_directory(source/platform platform_source)
add_library(platform-user ${sys_source} ${user_source} ${platform_source})
link_libraries(platform-user)

aux_source_directory(source/platform/alter stl_alter_source)
add_library(stl_alter ${stl_alter_source})

add_executable(cxx-test tests/cxx-test.cc)
# target_link_libraries(cxx-test cJSON platform-user)

add_custom_target(doxygen
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMAND doxygen
    COMMENT "building api reference"
)


set_target_properties(platform-user PROPERTIES PUBLIC_HEADER "include/platform.h")
set_target_properties(platform-user PROPERTIES PUBLIC_HEADER "include/platform-types.h")

install(TARGETS platform-user stl_alter
        LIBRARY DESTINATION lib
        PUBLIC_HEADER DESTINATION include
)

# include drivers
add_custom_target(install_drivers)

include(cmake/drivers/dummy.cmake)