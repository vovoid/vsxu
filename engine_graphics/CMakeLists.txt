cmake_minimum_required(VERSION 2.6)

include_directories(
  "../"
  "include"
  "../engine/include"
)

if(WIN32)
  include_directories(
    ../lib/win32
	../lib/win32/png
	../lib/win32/jpg
	../lib/win32/zlib
  )
  add_definitions(-DVSX_GLPNG_DLL)
  file(GLOB PNG_SOURCES "../lib/win32/png/*.c")
  file(GLOB JPG_SOURCES "../lib/win32/jpg/*.c")
  file(GLOB ZLIB_SOURCES "../lib/win32/zlib/*.c")
endif(WIN32)

#definitions
#if you want an engine without OpenGL linkage, add -DVSX_NO_GL
add_definitions(
 -DVSX_ENG_DLL=1
 -DVSX_FONT_NO_FT
 -DVSXU_EXE
 -DVSXU_DEVELOPER
 -DCMAKE_INSTALL_PREFIX="${CMAKE_INSTALL_PREFIX}"
)

set(module_id vsxu_engine_graphics)
message("*******************************************")
message("configuring            " ${module_id})
message("*******************************************")

set(SOURCES
src/gl_helper.cpp
src/glpng.cpp
src/jpg.cpp
src/logo_intro.cpp
src/render_texture.cpp
src/vsx_font.cpp
src/vsx_texture.cpp
src/vsx_statelist.cpp
src/vsx_manager.cpp
)
include(../cmake_globals.txt)

project (${module_id})

add_library(${module_id} SHARED ${SOURCES} ${EXTRA_LIBS} ${PNG_SOURCES} ${JPG_SOURCES} ${ZLIB_SOURCES})

install(TARGETS ${module_id} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

if(WIN32)
target_link_libraries(${module_id} gdi32 glew opengl32 glu32 pthreadGCE)
link_directories("../engine")
else(WIN32)
target_link_libraries(${module_id} X11 GLEW GL GLU)
endif(WIN32)
target_link_libraries(${module_id} vsxu_engine)
#include(../cmake_suffix.txt)

