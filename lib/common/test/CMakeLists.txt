cmake_minimum_required(VERSION 2.8)

include(${CMAKE_SOURCE_DIR}/cmake/CMakeFindLib.txt)

find_package(Threads REQUIRED)

if (NOT MSVC)
  add_definitions(
    -g
   -Wtype-limits
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

if (NOT WIN32)
    set(RT_LIBRARY rt)
else()
    set(RT_LIBRARY "")
endif()

add_executable(test_cpu_info test_cpu_info.cpp )

add_executable(test_mmap test_mmap.cpp )
target_link_libraries(test_mmap ${RT_LIBRARY} vsx_common vsx_compression)

add_executable(test_fread test_fread.cpp )
target_link_libraries(test_fread ${RT_LIBRARY} vsx_common vsx_compression )

add_executable(test_filesystem_archive_vsx test_filesystem_archive_vsx.cpp )
target_link_libraries(test_filesystem_archive_vsx ${RT_LIBRARY} vsx_common vsx_compression ${CMAKE_THREAD_LIBS_INIT})

add_executable(test_filesystem_archive_vsxz test_filesystem_archive_vsxz.cpp )
target_link_libraries(test_filesystem_archive_vsxz ${RT_LIBRARY} vsx_common vsx_compression ${CMAKE_THREAD_LIBS_INIT})

add_executable(test_string test_string.cpp )
target_link_libraries(test_string ${RT_LIBRARY} vsx_common ${CMAKE_THREAD_LIBS_INIT})

add_executable(test_thread_pool test_thread_pool.cpp )
target_link_libraries(test_thread_pool ${RT_LIBRARY} vsx_common vsx_compression ${CMAKE_THREAD_LIBS_INIT})

add_executable(test_vsx_filesystem_tree_writer test_vsx_filesystem_tree_writer.cpp )
target_link_libraries(test_vsx_filesystem_tree_writer ${RT_LIBRARY} vsx_common ${CMAKE_THREAD_LIBS_INIT})

add_executable(test_vsx_perf test_vsx_perf.cpp )
target_link_libraries(test_vsx_perf ${RT_LIBRARY} vsx_common ${CMAKE_THREAD_LIBS_INIT})

add_executable(test_vsx_input_event_queue test_vsx_input_event_queue.cpp )
target_link_libraries(test_vsx_input_event_queue ${RT_LIBRARY} vsx_common ${CMAKE_THREAD_LIBS_INIT})

add_executable(test_command_list test_command_list.cpp )
target_link_libraries(test_command_list vsx_common vsx_compression vsx_engine vsx_engine_graphics ${CMAKE_THREAD_LIBS_INIT} ${RT_LIBRARY})

add_executable(test_json test_json.cpp )
target_link_libraries(test_json vsx_common vsx_compression vsx_engine vsx_engine_graphics ${CMAKE_THREAD_LIBS_INIT} ${RT_LIBRARY})
