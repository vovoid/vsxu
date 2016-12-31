cmake_minimum_required(VERSION 2.6)

get_filename_component(list_file_path ${CMAKE_CURRENT_LIST_FILE} PATH)
string(REGEX MATCH "[a-z0-9._-]*$" module_id ${list_file_path})

message("configuring            " ${module_id})

project (${module_id})

include_directories(
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}/lib/common/include
  ${CMAKE_SOURCE_DIR}/lib/engine/include
  ${CMAKE_SOURCE_DIR}/lib/engine_graphics/include
)

include(${CMAKE_SOURCE_DIR}/cmake/CMakeFindLib.txt)


set(OS_SOURCES "")

#definitions
add_definitions(

 -g
 -DCMAKE_INSTALL_PREFIX="${CMAKE_INSTALL_PREFIX}"
)


find_package( Threads )

set(SOURCES
  main.cpp
)


add_executable(${module_id} ${SOURCES})

include(${CMAKE_SOURCE_DIR}/cmake_suffix.txt)

set_target_properties(
  ${module_id}
    PROPERTIES
      OUTPUT_NAME
        ${module_id}
)

install(TARGETS ${module_id} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
