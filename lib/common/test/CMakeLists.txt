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
)

add_executable(test_cpu_info test_cpu_info.cpp )

add_executable(test_mmap test_mmap.cpp )
target_link_libraries(test_mmap rt vsx_common vsx_compression)

add_executable(test_fread test_fread.cpp )
target_link_libraries(test_fread rt vsx_common vsx_compression )

add_executable(test_filesystem_archive_vsx test_filesystem_archive_vsx.cpp )
target_link_libraries(test_filesystem_archive_vsx rt vsx_common vsx_compression ${CMAKE_THREAD_LIBS_INIT})

add_executable(test_filesystem_archive_vsxz test_filesystem_archive_vsxz.cpp )
target_link_libraries(test_filesystem_archive_vsxz rt vsx_common vsx_compression ${CMAKE_THREAD_LIBS_INIT})

add_executable(test_string test_string.cpp )
target_link_libraries(test_string rt vsx_common ${CMAKE_THREAD_LIBS_INIT})

add_executable(test_thread_pool test_thread_pool.cpp )
target_link_libraries(test_thread_pool rt vsx_common ${CMAKE_THREAD_LIBS_INIT})

add_executable(test_vsx_filesystem_tree_writer test_vsx_filesystem_tree_writer.cpp )
target_link_libraries(test_vsx_filesystem_tree_writer rt vsx_common ${CMAKE_THREAD_LIBS_INIT})

add_executable(test_vsx_perf test_vsx_perf.cpp )
target_link_libraries(test_vsx_perf rt vsx_common ${CMAKE_THREAD_LIBS_INIT})

add_executable(test_vsx_input_event_queue test_vsx_input_event_queue.cpp )
target_link_libraries(test_vsx_input_event_queue rt vsx_common ${CMAKE_THREAD_LIBS_INIT})
