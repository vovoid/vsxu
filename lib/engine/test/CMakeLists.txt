cmake_minimum_required(VERSION 2.8)

include(${CMAKE_SOURCE_DIR}/cmake/CMakeFindLib.txt)

find_package(Threads REQUIRED)

add_definitions(
  -g
  -Wtype-limits
  -Wuninitialized
  -Wignored-qualifiers
  -Wempty-body
  -Wunused-parameter
)

if(WIN32)
  add_definitions( -Wno-error ) # vsx_filesystem::filesystemst WIN32
endif(WIN32)

include_directories(
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}/lib/common/include
  ${CMAKE_SOURCE_DIR}/lib/engine/include
)

add_executable(test_command_list test_command_list.cpp )
target_link_libraries(test_command_list vsx_common vsx_compression vsx_engine vsx_engine_graphics ${CMAKE_THREAD_LIBS_INIT} rt)

