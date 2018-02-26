find_package (Threads REQUIRED)
find_package (OpenSSL REQUIRED)

include_directories ("{PROJECT_BUILD_DIR}")

set (src
  api.h
  api.c
  rsa.h
  rsa.c
)

add_library (IOClient SHARED ${src})
target_link_libraries (IOClient
PUBLIC
  IO
  pthread # Threads
  crypto # OpenSSL
)


#add_executable (IOClientTest main.c)
#target_link_libraries (IOClientTest
#PRIVATE
#  IO
#  pthread
#)
