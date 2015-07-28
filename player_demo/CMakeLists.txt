set(module_id vsxu_demo_player)

message("configuring            " ${module_id})


################################################################################
project (${module_id})

################################################################################
# CMAKE PACKAGES ###############################################################
################################################################################
find_package(GLFW REQUIRED)
find_package(Threads REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

INCLUDE(FindPkgConfig)
PKG_SEARCH_MODULE(FREETYPE REQUIRED freetype2)
PKG_SEARCH_MODULE(PNG REQUIRED  libpng)
PKG_SEARCH_MODULE(ZLIB REQUIRED zlib)
PKG_SEARCH_MODULE(JPEG REQUIRED libjpeg)

include_directories(
  ${FREETYPE_INCLUDE_DIRS}
  ${PNG_INCLUDE_DIRS}
  ${ZLIB_INCLUDE_DIRS}
  ${JPEG_INCLUDE_DIRS}
)

link_directories(
  ${FREETYPE_LIBRARY_DIRS}
  ${PNG_LIBRARY_DIRS}
  ${ZLIB_LIBRARY_DIRS}
  ${JPEG_LIBRARY_DIRS}
)


################################################################################
# INCLUDES #####################################################################
################################################################################
include_directories(
  ${OPENGL_INCLUDE_DIR}
  ${GLEW_INCLUDE_PATH}
  ${CMAKE_SOURCE_DIR}/
  ${CMAKE_SOURCE_DIR}/engine/include
  ${CMAKE_SOURCE_DIR}/engine_graphics/include
  ${CMAKE_SOURCE_DIR}/player/src
)

################################################################################
# DEFINES ######################################################################
################################################################################

add_definitions(
 -DVSX_FONT_NO_FT
 -DDEBUG
 -DVSXU_DEVELOPER
 -DVSXU_ARTISTE
 -DCMAKE_INSTALL_PREFIX="${CMAKE_INSTALL_PREFIX}"
)


if (VSXU_DEBUG EQUAL 1)
  add_definitions( -DVSXU_DEBUG )
endif (VSXU_DEBUG EQUAL 1)

################################################################################
# SOURCES ######################################################################
################################################################################

set(OS_SOURCES "")

if(WIN32)
  set(OS_SOURCES
    src/main/win32/main.cpp
    src/main/win32/includes/nehegl.cpp
  )
endif(WIN32)

if(UNIX)
  set(OS_SOURCES
    src/main/linux/main.cpp
  )
endif(UNIX)

set(SOURCES
  src/application.cpp
  )

################################################################################
# LINK #########################################################################
################################################################################

add_executable(${module_id} ${SOURCES} ${OS_SOURCES})
include(../cmake_suffix.txt)

if(UNIX)
  target_link_libraries(
    ${module_id}
    ${GLFW_LIBRARY}
    ${CMAKE_THREAD_LIBS_INIT}
    ${GLEW_LIBRARY}
    ${OPENGL_LIBRARIES}
    ${PNG_LIBRARIES}
    ${JPEG_LIBRARIES}
    Xrandr
    vsxu_engine_graphics
    vsxu_engine
  )
  install(TARGETS ${module_id} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
  install(DIRECTORY ../share/visuals_faders ../share/visuals_player ../share/font DESTINATION ${CMAKE_INSTALL_PREFIX}/share/vsxu)
endif(UNIX)

if(WIN32)
  target_link_libraries(${module_id}
  wsock32
  ws2_32
  ${CMAKE_THREAD_LIBS_INIT}
  vsxu_engine_graphics
  vsxu_engine
  gdi32
)
endif()

################################################################################
## INSTALL #####################################################################
################################################################################

install(TARGETS ${module_id} DESTINATION ${VSXU_INSTALL_BIN_DIR} COMPONENT artiste)

IF (UNIX)
  CONFIGURE_FILE (${CMAKE_CURRENT_SOURCE_DIR}/vsxu-demo-player-fullscreen.desktop.in
                  ${CMAKE_CURRENT_BINARY_DIR}/vsxu-demo-player-fullscreen.desktop
                  @ONLY)
  CONFIGURE_FILE (${CMAKE_CURRENT_SOURCE_DIR}/vsxu-demo-player.desktop.in
                  ${CMAKE_CURRENT_BINARY_DIR}/vsxu-demo-player.desktop
                  @ONLY)

  INSTALL (FILES ${CMAKE_CURRENT_BINARY_DIR}/vsxu-demo-player-fullscreen.desktop
                 ${CMAKE_CURRENT_BINARY_DIR}/vsxu-demo-player.desktop
           DESTINATION ${CMAKE_INSTALL_PREFIX}/share/applications/)
ENDIF (UNIX)
