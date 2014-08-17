cmake_minimum_required(VERSION 2.6)

set(OS_SOURCES "")

#definitions
add_definitions(
 -DCMAKE_INSTALL_PREFIX="${CMAKE_INSTALL_PREFIX}"
 -Wl,-subsystem,windows
)


set(SOURCES
  main.cpp
)


add_executable(win_launcher ${SOURCES})

SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -mwindows")
set_target_properties(
  ${module_id}
    PROPERTIES
      OUTPUT_NAME
        win_launcher
)

