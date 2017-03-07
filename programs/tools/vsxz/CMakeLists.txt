cmake_minimum_required(VERSION 2.6)

include_directories(
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}/lib/common/include
)

include(${CMAKE_SOURCE_DIR}/cmake/CMakeFindLib.txt)

set(OS_SOURCES "")

if(VSXU_DEBUG)
add_definitions(
 -DDEBUG
)
endif(VSXU_DEBUG)

#definitions
add_definitions(
 -DCMAKE_INSTALL_PREFIX="${CMAKE_INSTALL_PREFIX}"
)

if (VSXU_STATIC EQUAL 1)
  add_definitions(-DVSXU_STATIC)
endif()

file(GLOB_RECURSE HEADER_FILES *.h)


get_filename_component(list_file_path ${CMAKE_CURRENT_LIST_FILE} PATH)
string(REGEX MATCH "[a-z._-]*$" module_id ${list_file_path})

message("configuring            " ${module_id})

find_package( Threads )

set(SOURCES
  main.cpp
)

project (${module_id})

add_executable(${module_id}  ${SOURCES} ${HEADER_FILES})

include(${CMAKE_SOURCE_DIR}/cmake_suffix.txt)

set_target_properties(
  ${module_id}
    PROPERTIES
      OUTPUT_NAME
        vsxz
)

target_link_libraries(${module_id}
  vsx_common
  vsx_compression

  ${CMAKE_THREAD_LIBS_INIT}
)

if(UNIX)
  target_link_libraries(${module_id}
    rt
  )
endif(UNIX)

install(TARGETS ${module_id} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
