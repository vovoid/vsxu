cmake_minimum_required(VERSION 2.6)
include(../../cmake_globals.txt)
include_directories(
  ../../
  ../../engine/include
)

set(OS_SOURCES "")

if(VSXU_DEBUG)
add_definitions(
 -DDEBUG
)
endif(VSXU_DEBUG)

#definitions
add_definitions(
 -DVSXU_EXE
 -DCMAKE_INSTALL_PREFIX="${CMAKE_INSTALL_PREFIX}"
)

get_filename_component(list_file_path ${CMAKE_CURRENT_LIST_FILE} PATH)
string(REGEX MATCH "[a-z._-]*$" module_id ${list_file_path})
message("*******************************************")
message("configuring            " ${module_id})
message("*******************************************")

set(SOURCES
  main.cpp
)

link_directories(
../../engine
)

project (${module_id})

add_executable(${module_id}  ${SOURCES} ${EXTRA_LIBS})
include(../../cmake_suffix.txt)

set_target_properties(
  ${module_id}
    PROPERTIES
      OUTPUT_NAME
        vsxz
)

if(UNIX)
  target_link_libraries(${module_id}
    dl
    vsxu_engine
  )
  install(TARGETS ${module_id} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
endif(UNIX)


if(WIN32)
  target_link_libraries(${module_id}
    vsxu_engine
  )
endif(WIN32)
