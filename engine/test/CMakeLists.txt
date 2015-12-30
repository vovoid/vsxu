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
  add_definitions( -Wno-error ) # vsx_filesystem::filesystemst WIN32
endif(WIN32)

include_directories(
${CMAKE_SOURCE_DIR}/../include
${CMAKE_SOURCE_DIR}/../../
 )

add_executable(test_cpu_info test_cpu_info.cpp )

add_executable(test_mmap test_mmap.cpp )
target_link_libraries(test_mmap rt vsxu_engine)

add_executable(test_fread test_fread.cpp )
target_link_libraries(test_fread rt vsxu_engine)

