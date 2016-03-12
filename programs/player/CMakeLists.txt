set(module_id vsxu_player)

message("configuring            " ${module_id})


################################################################################
project (${module_id})


include(${CMAKE_SOURCE_DIR}/cmake/CMakeFindLib.txt)

################################################################################
# CMAKE PACKAGES ###############################################################
################################################################################
find_package(Threads REQUIRED)
find_package(OpenGL REQUIRED)

PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)

include_directories(
  ${SDL2_INCLUDE_DIRS}
)

link_directories(
  ${SDL2_LIBRARY_DIRS}
)


################################################################################
# INCLUDES #####################################################################
################################################################################

include_directories(
  ${OPENGL_INCLUDE_DIR}
  ${CMAKE_SOURCE_DIR}/
  ${CMAKE_SOURCE_DIR}/lib/common/include
  ${CMAKE_SOURCE_DIR}/lib/engine/include
  ${CMAKE_SOURCE_DIR}/lib/engine_graphics/include
  ${CMAKE_SOURCE_DIR}/lib/engine_audiovisual/include
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
  src/player_control.cpp
  src/player_overlay.cpp
)

################################################################################
# LINK #########################################################################
################################################################################

add_executable(${module_id} ${SOURCES} ${HEADER_FILES})
include(${CMAKE_SOURCE_DIR}/cmake_suffix.txt)

if(UNIX)
  target_link_libraries(
    ${module_id}
    ${GLFW_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
    ${GLEW_LIBRARIES}
    ${OPENGL_LIBRARIES}
    ${PNG_LIBRARIES}
    ${ZLIB_LIBRARIES}
    ${JPEG_LIBRARIES}
    ${SDL2_LIBRARIES}
    vsx_engine_audiovisual
    vsx_engine_graphics
    vsx_engine
    vsx_common
    vsx_compression
    rt
  )
endif()

if(WIN32)
  target_link_libraries(
   ${module_id}
   -static
    ${GLFW_LIBRARIES}
    wsock32
    ws2_32
    vsx_engine_audiovisual
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
    ${SDL2_LIBRARIES}
)
endif()

if (VSXU_STATIC EQUAL 1)
  add_definitions(-DVSXU_STATIC)
endif()



if (VSXU_STATIC EQUAL 1)
  include(../plugins/static_factory_cmake_target_link_libraries)
endif()


################################################################################
## INSTALL #####################################################################
################################################################################

install(TARGETS ${module_id} DESTINATION ${VSXU_INSTALL_BIN_DIR} COMPONENT player)
install(
  DIRECTORY
    ${CMAKE_SOURCE_DIR}/share/visuals_faders
    ${CMAKE_SOURCE_DIR}/share/visuals_player
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


# FIXME: should have a "if(INSTALL_EXE)" ...
#if (WIN32)
#  install(FILES ${CMAKE__SOURCE_DIR}/../vsxu_player_launcher/vsxu_player_launcher.exe DESTINATION ${VSXU_INSTALL_BIN_DIR} COMPONENT player)
#endif()
#

