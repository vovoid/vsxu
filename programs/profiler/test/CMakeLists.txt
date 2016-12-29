cmake_minimum_required(VERSION 2.8)



find_package(Threads REQUIRED)

if (NOT MSVC)
  add_definitions(
    -g
    -Wtype-limits
    #-fpermissive # UNIX only, and should no longer be needed
    -Wuninitialized
    -Wignored-qualifiers
    -Wempty-body
    -Wunused-parameter
  )
endif()

include_directories(
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}/lib/common/include
)

add_executable(test_vsx_profiler test_vsx_profiler.cpp )

target_link_libraries( test_vsx_profiler ${CMAKE_THREAD_LIBS_INIT})

