set(module_id vsxu_engine_graphics)
message("*******************************************")
message("configuring            " ${module_id})
message("*******************************************")

project (${module_id})

################################################################################
# CMAKE PACKAGES ###############################################################
################################################################################
include(cmake_packages.txt)

################################################################################
# INCLUDES #####################################################################
################################################################################

include_directories(
  ${CMAKE_SOURCE_DIR}/
  ${CMAKE_SOURCE_DIR}/engine_graphics/include
  ${CMAKE_SOURCE_DIR}/engine/include
)

if(WIN32)
  include_directories(
    ../lib/win32
	  ../lib/win32/png
	  ../lib/win32/jpg
	  ../lib/win32/zlib
  )
  add_definitions(-DVSX_GLPNG_DLL -DVSX_ENG_DLL=1)
  file(GLOB PNG_SOURCES "../lib/win32/png/*.c")
  file(GLOB JPG_SOURCES "../lib/win32/jpg/*.c")
  file(GLOB ZLIB_SOURCES "../lib/win32/zlib/*.c")
endif(WIN32)

#definitions
add_definitions(
 -DVSX_ENG_DLL
 -DVSX_FONT_NO_FT
 -DVSXU_EXE
 -DVSXU_DEVELOPER
 -DCMAKE_INSTALL_PREFIX="${CMAKE_INSTALL_PREFIX}"
)



if (VSXU_DEBUG EQUAL 1)
  add_definitions( -DVSXU_DEBUG -DDEBUG )
endif (VSXU_DEBUG EQUAL 1)


if (PLATFORM_SHARED_FILES_FLAT_INSTALL EQUAL 1)
  add_definitions( -DPLATFORM_SHARED_FILES_FLAT_INSTALL )
endif (PLATFORM_SHARED_FILES_FLAT_INSTALL EQUAL 1)

set(SOURCES
  src/glpng.cpp
  src/jpg.cpp
  src/logo_intro.cpp
  src/render_texture.cpp
  src/vsx_font.cpp
  src/vsx_texture.cpp
  src/vsx_statelist.cpp
  src/vsx_manager.cpp
)

if (VSXU_OPENGL_ES_2_0 EQUAL 1)
  add_definitions(-DVSXU_OPENGL_ES -DVSXU_OPENGL_ES_2_0 -DVSX_TEXTURE_NO_RT_PBUFFER)
  set(SOURCES
    ${SOURCES}
    src/vsx_gl_es_shader_wrapper.cpp
  )
endif (VSXU_OPENGL_ES_2_0 EQUAL 1)



add_library(${module_id} SHARED ${SOURCES} ${PNG_SOURCES} ${JPG_SOURCES} ${ZLIB_SOURCES})
install(TARGETS ${module_id} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
if(WIN32)
  target_link_libraries(${module_id} gdi32)
  link_directories("../engine")
else(WIN32)

  if (VSXU_OPENGL_ES_2_0 EQUAL 1)
#    target_link_libraries(${module_id})
  else (VSXU_OPENGL_ES_2_0 EQUAL 1)
#    target_link_libraries(${module_id} X11 GLEW GL GLU)
  endif (VSXU_OPENGL_ES_2_0 EQUAL 1)
#  target_link_libraries(${module_id} X11 GLEW.a GL GLU png.a jpeg.a z.a)
endif(WIN32)

target_link_libraries(${module_id}
  ${OPENGL_LIBRARIES}
  ${CMAKE_THREAD_LIBS_INIT}
  ${PNG_LIBRARY}
  ${JPEG_LIBRARY}
  ${GLEW_LIBRARY}
  vsxu_engine
)
