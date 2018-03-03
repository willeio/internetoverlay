include_directories ("{PROJECT_BUILD_DIR}")

set (src
  shared.h
  #shared.c
  client.h
  client.c
  watcher.h
  watcher.c
  refresh.h
  refresh.c
  node.h
  node.c
)

add_executable (IONode ${src} main.c)
target_link_libraries (IONode
PRIVATE
  IO
  pthread
)
