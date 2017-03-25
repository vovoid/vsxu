cmake_minimum_required(VERSION 2.8)

  add_definitions(
    -g
    -Wall
    -Wtype-limits
    -fpermissive
    -Wuninitialized
    -Wignored-qualifiers
    -Wempty-body
    -Wunused-parameter
  )

include_directories(
${CMAKE_SOURCE_DIR}/../include
${CMAKE_SOURCE_DIR}/../../
 )

add_executable(test_vsx_profiler test_vsx_profiler.cpp )

add_executable(test_cpu_info test_cpu_info.cpp )
add_executable(test_vsx_profiler_additions test_vsx_profiler_additions.cpp )

target_link_libraries( test_vsx_profiler  pthread )
target_link_libraries( test_vsx_profiler_additions  pthread )
