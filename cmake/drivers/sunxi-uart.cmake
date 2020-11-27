cmake_minimum_required(VERSION 3.10)

set(driname sunxi-uart)
include_directories(drivers/${driname})
aux_source_directory(drivers/${driname} driver_source)
add_executable(${driname} drivers/foo.cxx ${driver_source})

add_custom_target(install_sunxi_uart
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/CMakeFiles/${driname}.dir/drivers/${driname}
    DEPENDS ${driname}
    COMMENT "copying ${driname} obj..."
    COMMAND mkdir -p ${CMAKE_INSTALL_PREFIX}/obj && cp *.obj ${CMAKE_INSTALL_PREFIX}/obj)
add_dependencies(install_drivers install_sunxi_uart)

