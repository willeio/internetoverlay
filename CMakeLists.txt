cmake_minimum_required (VERSION 2.6)
project (IONet C)

set (IO_VER_MAIN 0)
set (IO_VER_SUB  21)

configure_file (
  "${PROJECT_SOURCE_DIR}/cfg.h.in"
  "${PROJECT_BINARY_DIR}/cfg.h"
)

include_directories ("{PROJECT_BUILD_DIR}")
include_directories (".")

set (CMAKE_C_FLAGS "-std=c99 -g") # debug

add_subdirectory(IOLib)
add_subdirectory(IOSock)

option (BUILD_IO_MASTER
  "Build IOMaster" ON)

if (BUILD_IO_MASTER)
  add_subdirectory(IOMaster)
endif()


option (BUILD_IO_NODE
  "Build IONode" ON)

if (BUILD_IO_NODE)
  add_subdirectory(IONode)
endif()


option (BUILD_IO_CLIENT
  "Build IOClient" ON)

if (BUILD_IO_CLIENT)
  add_subdirectory(IOClient)
endif()


option (BUILD_IO_PROTOCOL
  "Build IOProtocol" ON)

if (BUILD_IO_PROTOCOL)
  add_subdirectory(IOProtocol)
endif()
