set(module_id vsxu_engine_graphics)

message("configuring            " ${module_id})


project (${module_id})

################################################################################
# CMAKE PACKAGES ###############################################################
################################################################################
include(cmake_packages.txt)

################################################################################
# INCLUDES #####################################################################
################################################################################

include_directories(
  ${OPENGL_INCLUDE_DIR}
  ${PNG_INCLUDE_DIR}
  ${JPEG_INCLUDE_DIR}
  ${CMAKE_SOURCE_DIR}/
  ${CMAKE_SOURCE_DIR}/engine_graphics/include
  ${CMAKE_SOURCE_DIR}/engine/include
)


################################################################################
# DEFINITIONS ##################################################################
################################################################################

if(WIN32)
  add_definitions(-DVSX_GLPNG_DLL -DVSX_ENG_DLL=1)
endif(WIN32)

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

################################################################################
# SOURCES ######################################################################
################################################################################

set(SOURCES
  src/glpng.cpp
  src/jpg.cpp
  src/logo_intro.cpp
  src/vsx_font.cpp
  src/vsx_texture.cpp
  src/vsx_statelist.cpp
  src/vsx_manager.cpp
)

if (VSXU_OPENGL_ES_2_0 EQUAL 1)
  add_definitions(-DVSXU_OPENGL_ES -DVSXU_OPENGL_ES_2_0)
  set(SOURCES
    ${SOURCES}
    src/vsx_gl_es_shader_wrapper.cpp
  )
endif (VSXU_OPENGL_ES_2_0 EQUAL 1)


add_library(${module_id} SHARED ${SOURCES} ${PNG_SOURCES} ${JPG_SOURCES} ${ZLIB_SOURCES})
install(TARGETS ${module_id} DESTINATION ${VSXU_INSTALL_LIB_DIR} COMPONENT engine)

######################################################################################
# Install the Headers so as to install VSXu as a library
if (UNIX)
  file(GLOB HEADERS "include/*.h")
  install (FILES ${HEADERS} DESTINATION ${CMAKE_INSTALL_PREFIX}/${VSXU_INSTALL_INCLUDE_DIR}/engine_graphics COMPONENT Devel)
endif()
################################################################################


if(WIN32)
  target_link_libraries(${module_id} gdi32)
else(WIN32)

  if (VSXU_OPENGL_ES_2_0 EQUAL 1)
#    target_link_libraries(${module_id})
  else (VSXU_OPENGL_ES_2_0 EQUAL 1)
#    target_link_libraries(${module_id} X11 GLEW GL GLU)
  endif (VSXU_OPENGL_ES_2_0 EQUAL 1)
#  target_link_libraries(${module_id} X11 GLEW.a GL GLU png.a jpeg.a z.a)
endif(WIN32)

if (NOT ${CMAKE_THREAD_LIBS_INIT}) # and crosscompile
  set(CMAKE_THREAD_LIBS_INIT pthread)
endif (NOT ${CMAKE_THREAD_LIBS_INIT})

target_link_libraries(${module_id}
  ${PNG_LIBRARIES}
  ${JPEG_LIBRARIES}
  ${OPENGL_LIBRARIES}
  ${CMAKE_THREAD_LIBS_INIT}
  ${GLEW_LIBRARY}
  vsxu_engine
)

if (WIN32)
  target_link_libraries(${module_id}
    gdi32
  )
endif()
