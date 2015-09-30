set(module_id vsxu_engine_graphics)

message("configuring            " ${module_id})


project (${module_id})

include(${CMAKE_SOURCE_DIR}/cmake/CMakeFindLib.txt)

################################################################################
# CMAKE PACKAGES ###############################################################
################################################################################
find_package(Threads REQUIRED)
find_package(OpenGL REQUIRED)


################################################################################
# INCLUDES #####################################################################
################################################################################

include_directories(
  ${OPENGL_INCLUDE_DIR}
  ${CMAKE_SOURCE_DIR}/
  ${CMAKE_SOURCE_DIR}/engine_graphics/include
  ${CMAKE_SOURCE_DIR}/engine_graphics/include/ftgl
  ${CMAKE_SOURCE_DIR}/engine/include
  ${CMAKE_SOURCE_DIR}/profiler/include
)

################################################################################
# DEFINITIONS ##################################################################
################################################################################

add_definitions(
   # -DBUILDING_DLL
 -DVSX_FONT_NO_FT
 -DVSXU_DEVELOPER
 -DCMAKE_INSTALL_PREFIX="${CMAKE_INSTALL_PREFIX}"
)

if (VSXU_STATIC EQUAL 1)
  add_definitions(-DVSXU_STATIC)
  add_definitions(-DGLEW_STATIC)
endif()


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
  src/vsx_font_outline.cpp
  src/vsx_texture.cpp
  src/vsx_gl_state.cpp
  src/gl_helper.cpp
  src/ftgl/FTBitmapGlyph.cpp
  src/ftgl/FTBufferGlyph.cpp
  src/ftgl/FTCharmap.cpp
  src/ftgl/FTContour.cpp
  src/ftgl/FTExtrdGlyph.cpp
  src/ftgl/FTFace.cpp
  src/ftgl/FTFont.cpp
  src/ftgl/FTGLBitmapFont.cpp
  src/ftgl/FTGLBufferFont.cpp
  src/ftgl/FTGLExtrdFont.cpp
  src/ftgl/FTGLGlyph.cpp
  src/ftgl/FTGLOutlineFont.cpp
  src/ftgl/FTGLPixmapFont.cpp
  src/ftgl/FTGLPolygonFont.cpp
  src/ftgl/FTGLTextureFont.cpp
  src/ftgl/FTGlyphContainer.cpp
  src/ftgl/FTLibrary.cpp
  src/ftgl/FTOutlineGlyph.cpp
  src/ftgl/FTPixmapGlyph.cpp
  src/ftgl/FTPoint.cpp
  src/ftgl/FTPolyGlyph.cpp
  src/ftgl/FTSize.cpp
  src/ftgl/FTTextureGlyph.cpp
  src/ftgl/FTVectoriser.cpp
)

file(GLOB_RECURSE HEADER_FILES *.h)

if (VSXU_OPENGL_ES_2_0 EQUAL 1)
  add_definitions(-DVSXU_OPENGL_ES -DVSXU_OPENGL_ES_2_0)
  set(SOURCES
    ${SOURCES}
    src/vsx_gl_es_shader_wrapper.cpp
  )
endif (VSXU_OPENGL_ES_2_0 EQUAL 1)


if (VSXU_STATIC EQUAL 1)  
  #static build  
  add_library(${module_id} STATIC ${SOURCES} ${HEADER_FILES})
else ()
  add_library(${module_id} SHARED ${SOURCES} ${HEADER_FILES})
endif()

install(TARGETS ${module_id} DESTINATION ${VSXU_INSTALL_LIB_DIR} COMPONENT engine)

######################################################################################
# Install the Headers so as to install VSXu as a library
if (UNIX OR VSXU_WINDOWS_LIB_BUILD)
  file(GLOB HEADERS "include/*.h")
  install (FILES ${HEADERS} DESTINATION ${CMAKE_INSTALL_PREFIX}/${VSXU_INSTALL_INCLUDE_DIR}/engine_graphics COMPONENT Devel)
  file(GLOB FTGL_HEADERS "include/ftgl/*.h")
  install (FILES ${FTGL_HEADERS} DESTINATION ${CMAKE_INSTALL_PREFIX}/${VSXU_INSTALL_INCLUDE_DIR}/engine_graphics/ftgl COMPONENT Devel)
  install(FILES ${CMAKE_SOURCE_DIR}/share/icons/vsxu.xpm DESTINATION ${CMAKE_INSTALL_PREFIX}/share/pixmaps/)
  install(FILES ${CMAKE_SOURCE_DIR}/share/icons/hicolor/16x16/apps/vsxu.png DESTINATION ${CMAKE_INSTALL_PREFIX}/share/pixmaps/hicolor/apps/16x16)
  install(FILES ${CMAKE_SOURCE_DIR}/share/icons/hicolor/22x22/apps/vsxu.png DESTINATION ${CMAKE_INSTALL_PREFIX}/share/pixmaps/hicolor/apps/22x22)
  install(FILES ${CMAKE_SOURCE_DIR}/share/icons/hicolor/24x24/apps/vsxu.png DESTINATION ${CMAKE_INSTALL_PREFIX}/share/pixmaps/hicolor/apps/24x24)
  install(FILES ${CMAKE_SOURCE_DIR}/share/icons/hicolor/32x32/apps/vsxu.png DESTINATION ${CMAKE_INSTALL_PREFIX}/share/pixmaps/hicolor/apps/32x32)
  install(FILES ${CMAKE_SOURCE_DIR}/share/icons/hicolor/36x36/apps/vsxu.png DESTINATION ${CMAKE_INSTALL_PREFIX}/share/pixmaps/hicolor/apps/36x36)
  install(FILES ${CMAKE_SOURCE_DIR}/share/icons/hicolor/48x48/apps/vsxu.png DESTINATION ${CMAKE_INSTALL_PREFIX}/share/pixmaps/hicolor/apps/48x48)
  install(FILES ${CMAKE_SOURCE_DIR}/share/icons/hicolor/64x64/apps/vsxu.png DESTINATION ${CMAKE_INSTALL_PREFIX}/share/pixmaps/hicolor/apps/64x64)
  install(FILES ${CMAKE_SOURCE_DIR}/share/icons/hicolor/72x72/apps/vsxu.png DESTINATION ${CMAKE_INSTALL_PREFIX}/share/pixmaps/hicolor/apps/72x72)
  install(FILES ${CMAKE_SOURCE_DIR}/share/icons/hicolor/96x96/apps/vsxu.png DESTINATION ${CMAKE_INSTALL_PREFIX}/share/pixmaps/hicolor/apps/96x96)
  install(FILES ${CMAKE_SOURCE_DIR}/share/icons/hicolor/128x128/apps/vsxu.png DESTINATION ${CMAKE_INSTALL_PREFIX}/share/pixmaps/hicolor/apps/128x128)
  install(FILES ${CMAKE_SOURCE_DIR}/share/icons/hicolor/192x192/apps/vsxu.png DESTINATION ${CMAKE_INSTALL_PREFIX}/share/pixmaps/hicolor/apps/192x192)
  install(FILES ${CMAKE_SOURCE_DIR}/share/icons/hicolor/256x256/apps/vsxu.png DESTINATION ${CMAKE_INSTALL_PREFIX}/share/pixmaps/hicolor/apps/256x256)
  install(FILES ${CMAKE_SOURCE_DIR}/share/icons/hicolor/512x512/apps/vsxu.png DESTINATION ${CMAKE_INSTALL_PREFIX}/share/pixmaps/hicolor/apps/512x512)
endif()
################################################################################


if(WIN32)
  target_link_libraries(${module_id} gdi32)
  target_link_libraries(${module_id} -static)
else(WIN32)

  if (VSXU_OPENGL_ES_2_0 EQUAL 1)
#    target_link_libraries(${module_id})
  else (VSXU_OPENGL_ES_2_0 EQUAL 1)
#    target_link_libraries(${module_id} X11 GLEW GL GLU)
  endif (VSXU_OPENGL_ES_2_0 EQUAL 1)
#  target_link_libraries(${module_id} X11 GLEW.a GL GLU png.a jpeg.a z.a)
endif(WIN32)


target_link_libraries(
  ${module_id}
  vsxu_engine
  ${GLEW_LIBRARIES}
  ${FREETYPE_LIBRARIES}
  ${CMAKE_THREAD_LIBS_INIT}
  ${OPENGL_LIBRARIES}
  ${PNG_LIBRARIES}
  ${ZLIB_LIBRARIES}
  ${JPEG_LIBRARIES}
)

