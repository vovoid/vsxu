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
  src/logo_intro.cpp
  src/vsx_font.cpp
  src/vsx_font_outline.cpp
  src/vsx_gl_state.cpp
  src/gl_helper.cpp
  src/bitmap/loaders/vsx_bitmap_loader_jpg.cpp
  src/bitmap/loaders/vsx_bitmap_loader_png.cpp
)

#ftgl
include_directories(
thirdparty/ftgl
thirdparty/ftgl/src
)


set( SOURCES_FREETYPE
    thirdparty/freetype2/src/autofit/autofit.c
    thirdparty/freetype2/src/base/ftbase.c
    thirdparty/freetype2/src/base/ftbbox.c
    thirdparty/freetype2/src/base/ftbdf.c
    thirdparty/freetype2/src/base/ftbitmap.c
    thirdparty/freetype2/src/base/ftcid.c
    thirdparty/freetype2/src/base/ftdebug.c
    thirdparty/freetype2/src/base/ftfstype.c
    thirdparty/freetype2/src/base/ftgasp.c
    thirdparty/freetype2/src/base/ftglyph.c
    thirdparty/freetype2/src/base/ftgxval.c
    thirdparty/freetype2/src/base/ftinit.c
    thirdparty/freetype2/src/base/ftlcdfil.c
    thirdparty/freetype2/src/base/ftmm.c
    thirdparty/freetype2/src/base/ftotval.c
    thirdparty/freetype2/src/base/ftpatent.c
    thirdparty/freetype2/src/base/ftpfr.c
    thirdparty/freetype2/src/base/ftstroke.c
    thirdparty/freetype2/src/base/ftsynth.c
    thirdparty/freetype2/src/base/ftsystem.c
    thirdparty/freetype2/src/base/fttype1.c
    thirdparty/freetype2/src/base/ftwinfnt.c
    thirdparty/freetype2/src/bdf/bdf.c
    thirdparty/freetype2/src/cff/cff.c
    thirdparty/freetype2/src/cid/type1cid.c
    thirdparty/freetype2/src/pcf/pcf.c
    thirdparty/freetype2/src/pfr/pfr.c
    thirdparty/freetype2/src/psaux/psaux.c
    thirdparty/freetype2/src/pshinter/pshinter.c
    thirdparty/freetype2/src/psnames/psnames.c
    thirdparty/freetype2/src/raster/raster.c
    thirdparty/freetype2/src/sfnt/sfnt.c
    thirdparty/freetype2/src/smooth/smooth.c
    thirdparty/freetype2/src/truetype/truetype.c
    thirdparty/freetype2/src/type1/type1.c
    thirdparty/freetype2/src/type42/type42.c
    thirdparty/freetype2/src/winfonts/winfnt.c
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
    thirdparty/ftgl/src/FTGlyph/FTGLGlyph.cpp
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

if (WIN32)
  include_directories( thirdparty/freetype2/include )
  include_directories( thirdparty/freetype2/include/freetype )
  include_directories( thirdparty/freetype2/include/freetype/config )
  add_definitions( -DFTGL_LIBRARY_STATIC -DFT2_BUILD_LIBRARY)
  set(SOURCES ${SOURCES} ${SOURCES_FREETYPE} thirdparty/glew-1.7.0/src/glew.c)
endif()

#perlin
include_directories(
thirdparty/perlin
)
set(SOURCES_PERLIN
thirdparty/perlin/perlin.cpp
)

#lodepng
include_directories(
    thirdparty/lodepng
)
set(SOURCES_LODEPNG
    thirdparty/lodepng/lodepng.cpp
)

#jpeg
include_directories(
thirdparty/mini-jpeg
)


file(GLOB_RECURSE HEADER_FILES *.h)


if (VSXU_STATIC EQUAL 1)  
  #static build
  add_library(${module_id} STATIC ${SOURCES} ${SOURCES_FTGL} ${SOURCES_PERLIN} ${HEADER_FILES} ${SOURCES_LODEPNG})
else ()
  add_library(${module_id} SHARED ${SOURCES} ${SOURCES_FTGL} ${SOURCES_PERLIN} ${HEADER_FILES} ${SOURCES_LODEPNG})
endif()

if (UNIX)
install(TARGETS ${module_id} DESTINATION ${VSXU_INSTALL_LIB_DIR} COMPONENT engine)
endif()

######################################################################################
# Install the Headers so as to install VSXu as a library
if (UNIX OR VSXU_WINDOWS_LIB_BUILD)
  install (DIRECTORY "include/" DESTINATION ${CMAKE_INSTALL_PREFIX}/${VSXU_INSTALL_INCLUDE_DIR}/engine_graphics COMPONENT Devel)
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
)

