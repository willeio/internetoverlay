set (src
  io.h
  io.c
  thread.h
  thread.c
  list.h
  list.c
)

add_library (IO SHARED ${src})
target_include_directories (IO PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}../")
target_link_libraries (IO
PUBLIC
  IOSock
)
