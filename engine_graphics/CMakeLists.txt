cmake_minimum_required(VERSION 2.6)

include_directories(
../
include
../engine/include
)

add_definitions(-Wall)
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
message("building            " ${module_id})
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

add_library(${module_id} SHARED ${SOURCES} ${EXTRA_LIBS})

install(TARGETS ${module_id} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
target_link_libraries(${module_id} X11 GLEW GL GLU vsxu_engine)
#include(../cmake_suffix.txt)

