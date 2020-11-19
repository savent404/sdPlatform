set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_C_COMPILER arm-linux-gnueabi-gcc)
set(CMAKE_CXX_COMPILER arm-linux-gnueabi-g++)
set(CMAKE_CXX_STANDARD 17)
add_compile_options(-fno-exceptions)
add_compile_options(-fno-rtti)
add_compile_options(-ffreestanding)
add_compile_options(-ffunction-sections)
add_compile_options(-fdata-sections)
add_compile_options(-Wall)
add_compile_options(-marm)
add_compile_options(-march=armv7-a)
add_compile_options(-mno-unaligned-access)
add_compile_options(-msoft-float)
add_compile_options(-fno-stack-protector)
add_compile_options(-fno-strict-overflow)
add_compile_options(-nostdlib)
# set(CMAKE_EXE_LINKER_FLAGS "-specs=nosys.specs")
add_compile_options(-fno-threadsafe-statics)