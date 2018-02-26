include_directories ("{PROJECT_BUILD_DIR}")

set (src
  shared.h
  client.h
  client.c
  watcher.h
  watcher.c
)

add_executable (IOMaster ${src} main.c)
target_link_libraries (IOMaster
PRIVATE
  IO
  pthread
)
