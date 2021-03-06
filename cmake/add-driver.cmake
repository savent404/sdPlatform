cmake_minimum_required(VERSION 3.14)

macro(ADD_DRIVERS curdir)
  file(GLOB children RELATIVE ${curdir} ${curdir}/*)
  set(dirlist "")
  foreach(child ${children})
    if(IS_DIRECTORY ${curdir}/${child})
      add_subdirectory(${curdir}/${child} drivers/${child})
    endif()
  endforeach()
endmacro()