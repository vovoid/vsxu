set(module_id vsxu_player)

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
find_package(PNG REQUIRED)
find_package(JPEG REQUIRED)

################################################################################
# INCLUDES #####################################################################
################################################################################
include_directories(
  ${OPENGL_INCLUDE_DIR}
  ${GLEW_INCLUDE_PATH}
  ${CMAKE_SOURCE_DIR}/
  ${CMAKE_SOURCE_DIR}/engine/include
  ${CMAKE_SOURCE_DIR}/profiler/include
  ${CMAKE_SOURCE_DIR}/engine_graphics/include
  ${CMAKE_SOURCE_DIR}/engine_audiovisual/include
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
  src/application.cpp
  src/vsx_overlay.cpp
)

################################################################################
# LINK #########################################################################
################################################################################

add_executable(${module_id} ${SOURCES})
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
    vsxu_engine_audiovisual
    vsxu_engine_graphics
    vsxu_engine
    rt
  )
endif()

if(WIN32)
  target_link_libraries(
  ${module_id}
    ${GLFW_LIBRARY}
    wsock32
    ws2_32
    ${CMAKE_THREAD_LIBS_INIT}
    vsxu_engine_audiovisual
    vsxu_engine_graphics
    vsxu_engine
    gdi32
    ${OPENGL_LIBRARIES}
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


if (WIN32)
  install(FILES ~/vsxu-dev/player_launcher/vsxu_player_launcher.exe DESTINATION ${VSXU_INSTALL_BIN_DIR} COMPONENT player)
endif()

