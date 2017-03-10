set(module_id vsxu_player)

message("configuring            " ${module_id})


################################################################################
project (${module_id})

include(${CMAKE_SOURCE_DIR}/cmake/CMakeFindLib.txt)
include(${CMAKE_SOURCE_DIR}/cmake/CMakeVSXuGfxLib.txt)

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
  ${CMAKE_SOURCE_DIR}/lib/application/include
  ${CMAKE_SOURCE_DIR}/lib/common/include
  ${CMAKE_SOURCE_DIR}/lib/engine/include
  ${CMAKE_SOURCE_DIR}/lib/engine_graphics/include
  ${CMAKE_SOURCE_DIR}/programs/player/src
)

file(GLOB_RECURSE HEADER_FILES *.h)

################################################################################
# DEFINES ######################################################################
################################################################################
add_definitions(
 -DVSX_FONT_NO_FT
 -DDEBUG
 -DVSXU_DEVELOPER
 -DVSXU_ARTISTE
 -DVSXU_MULTIMONITOR
 -DCMAKE_INSTALL_PREFIX="${CMAKE_INSTALL_PREFIX}"
)

if(VSXU_DEBUG EQUAL 1)
  add_definitions( -DVSXU_DEBUG )
endif()

################################################################################
# SOURCES ######################################################################
################################################################################

set(SOURCES
  src/main.cpp
)

################################################################################
# LINK #########################################################################
################################################################################

add_executable(${module_id} ${SOURCES} ${HEADER_FILES})
include(${CMAKE_SOURCE_DIR}/cmake_suffix.txt)

if(UNIX)
  if (VSXU_STATIC)
    target_link_libraries(
      ${module_id}
      ${CMAKE_THREAD_LIBS_INIT}
      ${GLEW_LIBRARIES}
      ${OPENGL_LIBRARIES}
      vsx_application
      ${VSXU_GFX_LIB_LIBS}
      vsx_common
      vsx_compression
      vsx_engine
      vsx_engine_graphics
      plugins
      rt
      vsx_common
      vsx_compression
      vsx_engine
      vsx_engine_graphics
      plugins
    )
  else()
    target_link_libraries(
      ${module_id}
      ${CMAKE_THREAD_LIBS_INIT}
      ${GLEW_LIBRARIES}
      ${OPENGL_LIBRARIES}
      vsx_application
      ${VSXU_GFX_LIB_LIBS}
      vsx_common
      vsx_compression
      vsx_engine
      vsx_engine_graphics
      rt
    )
  endif()
endif()

if(WIN32)
  target_link_libraries(
   ${module_id}
   wsock32
   ws2_32
   vsx_application
   vsx_engine_graphics
   vsx_engine
   vsx_common
   vsx_compression
   ${PNG_LIBRARIES}
   ${ZLIB_LIBRARIES}
   ${JPEG_LIBRARIES}
   gdi32
   ${CMAKE_THREAD_LIBS_INIT}
   ${OPENGL_LIBRARIES}
   ${VSXU_GFX_LIB_LIBS}
)
endif()

if (VSXU_STATIC EQUAL 1)
  add_definitions(-DVSXU_STATIC)
endif()



if (VSXU_STATIC EQUAL 1)
  target_link_libraries(${module_id} plugins)
endif()


################################################################################
## INSTALL #####################################################################
################################################################################

install(TARGETS ${module_id} DESTINATION ${VSXU_INSTALL_BIN_DIR} COMPONENT player)
install(
  DIRECTORY
    ${CMAKE_SOURCE_DIR}/share/visuals_faders
    ${CMAKE_SOURCE_DIR}/share/visuals_player
    ${CMAKE_SOURCE_DIR}/share/visuals_packs
    ${CMAKE_SOURCE_DIR}/share/font
  DESTINATION
    ${VSXU_INSTALL_SHARE_DIR}
  COMPONENT player
)


IF (UNIX)
  CONFIGURE_FILE (${CMAKE_CURRENT_SOURCE_DIR}/vsxu-player-fullscreen.desktop.in
                  ${CMAKE_CURRENT_BINARY_DIR}/vsxu-player-fullscreen.desktop
                  @ONLY)
  CONFIGURE_FILE (${CMAKE_CURRENT_SOURCE_DIR}/vsxu-player.desktop.in
                  ${CMAKE_CURRENT_BINARY_DIR}/vsxu-player.desktop
                  @ONLY)

  INSTALL (FILES ${CMAKE_CURRENT_BINARY_DIR}/vsxu-player-fullscreen.desktop
                 ${CMAKE_CURRENT_BINARY_DIR}/vsxu-player.desktop
           DESTINATION ${CMAKE_INSTALL_PREFIX}/share/applications/)
ENDIF (UNIX)



