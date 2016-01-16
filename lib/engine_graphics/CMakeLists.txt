set(module_id vsx_engine_graphics)

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
  ${CMAKE_SOURCE_DIR}/lib/common/include
  ${CMAKE_SOURCE_DIR}/lib/engine_graphics/include
  ${CMAKE_SOURCE_DIR}/lib/engine_graphics/include/ftgl
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
  src/vsx_gl_state.cpp
  src/gl_helper.cpp
)

include_directories(
thirdparty/ftgl
thirdparty/ftgl/src
)
set(SOURCES_FTGL
thirdparty/ftgl/src/FTBuffer.cpp
thirdparty/ftgl/src/FTCharmap.cpp
thirdparty/ftgl/src/FTCleanup.cpp
thirdparty/ftgl/src/FTContour.cpp
thirdparty/ftgl/src/FTFace.cpp
thirdparty/ftgl/src/FTFont/FTBitmapFont.cpp
thirdparty/ftgl/src/FTFont/FTBufferFont.cpp
thirdparty/ftgl/src/FTFont/FTExtrudeFont.cpp
thirdparty/ftgl/src/FTFont/FTFont.cpp
thirdparty/ftgl/src/FTFont/FTFontGlue.cpp
thirdparty/ftgl/src/FTFont/FTOutlineFont.cpp
thirdparty/ftgl/src/FTFont/FTPixmapFont.cpp
thirdparty/ftgl/src/FTFont/FTPolygonFont.cpp
thirdparty/ftgl/src/FTFont/FTTextureFont.cpp
thirdparty/ftgl/src/FTFont/FTTriangleExtractorFont.cpp
thirdparty/ftgl/src/FTGL.cpp
thirdparty/ftgl/src/FTGlyph/FTBitmapGlyph.cpp
thirdparty/ftgl/src/FTGlyph/FTBufferGlyph.cpp
thirdparty/ftgl/src/FTGlyph/FTExtrudeGlyph.cpp
thirdparty/ftgl/src/FTGlyph/FTGlyph.cpp
thirdparty/ftgl/src/FTGlyph/FTGlyphGlue.cpp
thirdparty/ftgl/src/FTGlyph/FTOutlineGlyph.cpp
thirdparty/ftgl/src/FTGlyph/FTPixmapGlyph.cpp
thirdparty/ftgl/src/FTGlyph/FTPolygonGlyph.cpp
thirdparty/ftgl/src/FTGlyph/FTTextureGlyph.cpp
thirdparty/ftgl/src/FTGlyph/FTTriangleExtractorGlyph.cpp
thirdparty/ftgl/src/FTGlyphContainer.cpp
thirdparty/ftgl/src/FTLayout/FTLayout.cpp
thirdparty/ftgl/src/FTLayout/FTLayoutGlue.cpp
thirdparty/ftgl/src/FTLayout/FTSimpleLayout.cpp
thirdparty/ftgl/src/FTLibrary.cpp
thirdparty/ftgl/src/FTPoint.cpp
thirdparty/ftgl/src/FTSize.cpp
thirdparty/ftgl/src/FTVectoriser.cpp
)

file(GLOB_RECURSE HEADER_FILES *.h)


if (VSXU_STATIC EQUAL 1)  
  #static build  
  add_library(${module_id} STATIC ${SOURCES} ${SOURCES_FTGL} ${HEADER_FILES})
else ()
  add_library(${module_id} SHARED ${SOURCES} ${SOURCES_FTGL} ${HEADER_FILES})
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
endif()


target_link_libraries(
  ${module_id}
  vsx_engine
  ${GLEW_LIBRARIES}
  ${FTGL_LIBRARY}
  ${FREETYPE_LIBRARIES}
  ${CMAKE_THREAD_LIBS_INIT}
  ${OPENGL_LIBRARIES}
  ${PNG_LIBRARIES}
  ${ZLIB_LIBRARIES}
  ${JPEG_LIBRARIES}
)

