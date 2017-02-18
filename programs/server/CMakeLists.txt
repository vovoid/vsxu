set(module_id vsxu_server)

message("configuring            " ${module_id})

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
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}/lib/application/include
  ${CMAKE_SOURCE_DIR}/lib/common/include
  ${CMAKE_SOURCE_DIR}/lib/engine/include
  ${CMAKE_SOURCE_DIR}/lib/engine_graphics/include
  src
)


################################################################################
# DEFINES ######################################################################
################################################################################

if(VSXU_DEBUG)
  add_definitions( -DDEBUG )
  add_definitions( -DVSXU_DEBUG )
endif(VSXU_DEBUG)

add_definitions(
 -DVSXU_ARTISTE
 -DVSX_FONT_NO_FT
 -DCMAKE_INSTALL_PREFIX="${CMAKE_INSTALL_PREFIX}"
)

if (VSXU_STATIC EQUAL 1)
  add_definitions(-DVSXU_STATIC)
endif()


################################################################################
# SOURCES ######################################################################
################################################################################

set(OS_SOURCES "")

if(UNIX)
    set(SOURCES
      src/main.cpp
    )


  add_executable(${module_id} ${SOURCES})
  include(${CMAKE_SOURCE_DIR}/cmake_suffix.txt)

  target_link_libraries(
    ${module_id}
    ${CMAKE_THREAD_LIBS_INIT}
    ${GLEW_LIBRARIES}
    ${PNG_LIBRARIES}
    ${JPEG_LIBRARIES}
    vsx_application
    ${VSXU_GFX_LIB_LIBS}
    rt
    vsx_common
    vsx_compression
    vsx_engine
    vsx_engine_graphics
  )
  if (VSXU_STATIC EQUAL 1)
    target_link_libraries(${module_id}
      plugins
      vsx_common
      vsx_compression
      vsx_engine
      vsx_engine_graphics
      plugins
    )
  endif (VSXU_STATIC EQUAL 1)

  install(TARGETS ${module_id} DESTINATION ${VSXU_INSTALL_BIN_DIR} COMPONENT server)

  CONFIGURE_FILE (${CMAKE_CURRENT_SOURCE_DIR}/vsxu-server-fullscreen.desktop.in
                  ${CMAKE_CURRENT_BINARY_DIR}/vsxu-server-fullscreen.desktop
                  @ONLY)
  CONFIGURE_FILE (${CMAKE_CURRENT_SOURCE_DIR}/vsxu-server.desktop.in
                  ${CMAKE_CURRENT_BINARY_DIR}/vsxu-server.desktop
                  @ONLY)

  INSTALL (FILES ${CMAKE_CURRENT_BINARY_DIR}/vsxu-server-fullscreen.desktop
                 ${CMAKE_CURRENT_BINARY_DIR}/vsxu-server.desktop
           DESTINATION ${CMAKE_INSTALL_PREFIX}/share/applications/)
ENDIF (UNIX)
