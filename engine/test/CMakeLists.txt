cmake_minimum_required(VERSION 2.8)

  add_definitions(
    -g
    -Wtype-limits
    #-fpermissive # UNIX only, and should no longer be needed
    -Wuninitialized
    -Wignored-qualifiers
    -Wempty-body
    -Wunused-parameter
  )
if(WIN32)
  add_definitions( -Wno-error ) # vsxfst WIN32
endif(WIN32)

include_directories(
${CMAKE_SOURCE_DIR}/../include
${CMAKE_SOURCE_DIR}/../../
 )

add_executable(test_vsx_profiler test_vsx_profiler.cpp )

add_executable(test_cpu_info test_cpu_info.cpp )

target_link_libraries( test_vsx_profiler  pthread )

