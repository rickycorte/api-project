cmake_minimum_required (VERSION 3.7)
project(cetrioli C)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Werror -Wall -g")

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Default build type is set to DEBUG" FORCE)
endif(NOT CMAKE_BUILD_TYPE)

include_directories(src)

add_executable(cetrioli
    src/main.c
    src/entity_tree.c)

set(
        CMAKE_RUNTIME_OUTPUT_DIRECTORY
        ${CMAKE_HOME_DIRECTORY}/bin
)


add_custom_target(run COMMAND cd bin && ./cetrioli -c DEPENDS cetrioli)