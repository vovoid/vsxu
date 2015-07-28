cmake_minimum_required(VERSION 2.6)
include(../../cmake_globals.txt)
include_directories(
  ../../
  ../../engine/include
  ../../engine_graphics/include
)

INCLUDE(FindPkgConfig)
PKG_SEARCH_MODULE(FREETYPE REQUIRED freetype2)
PKG_SEARCH_MODULE(PNG REQUIRED  libpng)
PKG_SEARCH_MODULE(ZLIB REQUIRED zlib)
PKG_SEARCH_MODULE(JPEG REQUIRED libjpeg)

include_directories(
  ${FREETYPE_INCLUDE_DIRS}
  ${PNG_INCLUDE_DIRS}
  ${ZLIB_INCLUDE_DIRS}
  ${JPEG_INCLUDE_DIRS}
)

link_directories(
  ${FREETYPE_LIBRARY_DIRS}
  ${PNG_LIBRARY_DIRS}
  ${ZLIB_LIBRARY_DIRS}
  ${JPEG_LIBRARY_DIRS}
)

set(OS_SOURCES "")

if(VSXU_DEBUG)
add_definitions(
 -DDEBUG
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


get_filename_component(list_file_path ${CMAKE_CURRENT_LIST_FILE} PATH)
string(REGEX MATCH "[a-z._-]*$" module_id ${list_file_path})

message("configuring            " ${module_id})

find_package( Threads )

set(SOURCES
  main.cpp
)

link_directories(
../../engine
../../engine_graphics
)

project (${module_id})

add_executable(${module_id}  ${SOURCES})
include(../../cmake_suffix.txt)

set_target_properties(
  ${module_id}
    PROPERTIES
      OUTPUT_NAME
        vsxl
)

target_link_libraries(${module_id}
  vsxu_engine
  vsxu_engine_graphics
  ${CMAKE_THREAD_LIBS_INIT}
  -lrt
)

set(CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_CXX_LINK_EXECUTABLE} -lrt")


install(TARGETS ${module_id} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
