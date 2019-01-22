include_directories ("{PROJECT_BUILD_DIR}")

set (src
  states.h
  prot.h
  prot.c
)

add_library (IOProtocol SHARED ${src})
target_include_directories (IOProtocol PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}../")
target_link_libraries (IOProtocol
PRIVATE
  IO
  IOClient
  pthread
)
