cmake_minimum_required(VERSION 2.6)

# compute module id from compilation path
get_filename_component(list_file_path ${CMAKE_CURRENT_LIST_FILE} PATH)
string(REGEX MATCH "[a-z0-9._-]*$" module_id ${list_file_path})

project (${module_id})

message("configuring vsxu module            " ${module_id})

if (WIN32)
include_directories( ${CMAKE_CURRENT_LIST_DIR}/../../../lib/engine_graphics/thirdparty/freetype2/include )
include_directories( ${CMAKE_CURRENT_LIST_DIR}/../../../lib/engine_graphics/thirdparty/freetype2/include/freetype )
include_directories( ${CMAKE_CURRENT_LIST_DIR}/../../../lib/engine_graphics/thirdparty/freetype2/include/freetype/config )
include_directories( ${CMAKE_CURRENT_LIST_DIR}/../../../lib/engine_graphics/thirdparty/ftgl)
include_directories( ${CMAKE_CURRENT_LIST_DIR}/../../../lib/engine_graphics/thirdparty/ftgl/src)
add_definitions( -DFTGL_LIBRARY_STATIC -DFT2_BUILD_LIBRARY)
endif()

# set defines for MOD_CM, MOD_DM, MOD_NM
CREATE_STATIC_GLUE(${module_id})

# find the local source files
file(GLOB SOURCES "*.cpp" "ftgl/*.cpp")
file(GLOB_RECURSE HEADER_FILES *.h)

include_directories(${CMAKE_CURRENT_LIST_DIR}/../../../lib/engine_graphics/thirdparty/ftgl/src)

# include common
include(../cmake_globals.txt)

# actual compilation target
CREATE_MODULE(${module_id})

# include common
include(../cmake_suffix.txt)
