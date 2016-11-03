cmake_minimum_required(VERSION 2.6)

get_filename_component(list_file_path ${CMAKE_CURRENT_LIST_FILE} PATH)
string(REGEX MATCH "[a-z._-]*$" module_id ${list_file_path})

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

if(VSXU_DEBUG)
add_definitions(
 -DDEBUG
 -DVSX_NO_GL=1
)
endif(VSXU_DEBUG)

#definitions
add_definitions(
 -DCMAKE_INSTALL_PREFIX="${CMAKE_INSTALL_PREFIX}"
 -lrt
)

if (VSXU_STATIC EQUAL 1)
  add_definitions(-DVSXU_STATIC)
endif()



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
        vsxl
)

target_link_libraries(
  ${module_id}
  ${CMAKE_THREAD_LIBS_INIT}
  vsx_common
  vsx_compression
  vsx_engine
  vsx_engine_graphics
  rt
  #vsx_widget
  vsx_common
  vsx_compression
  vsx_engine
  vsx_engine_graphics
)

  if (VSXU_STATIC EQUAL 1)
    target_link_libraries(${module_id}
      plugins
      vsx_engine
      vsx_common
      vsx_compression
      vsx_common
      vsx_engine_graphics
      plugins
      vsx_engine
      vsx_common
      vsx_compression
      vsx_common
      vsx_engine_graphics
    )
  endif (VSXU_STATIC EQUAL 1)



#set(CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_CXX_LINK_EXECUTABLE} -lrt")


install(TARGETS ${module_id} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
