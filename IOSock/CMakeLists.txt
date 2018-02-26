set (src_basic
  net.h
)

if(MINGW)
  set(src_platform
    windows.c
  )
else()
  set (src_platform
    nix.c
  )
endif()

add_library (IOSock SHARED ${src_basic} ${src_platform})
target_include_directories (IOSock PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}../")
