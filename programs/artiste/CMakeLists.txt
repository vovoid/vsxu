set(module_id vsxu_artiste)

message("configuring            " ${module_id})

include(${CMAKE_SOURCE_DIR}/cmake/CMakeFindLib.txt)
include(${CMAKE_SOURCE_DIR}/cmake/CMakeVSXuGfxLib.txt)

################################################################################
project (${module_id})

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
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}/lib/application/include
  ${CMAKE_SOURCE_DIR}/lib/common/include
  ${CMAKE_SOURCE_DIR}/lib/engine/include
  ${CMAKE_SOURCE_DIR}/lib/engine_graphics/include
  ${CMAKE_SOURCE_DIR}/lib/widget/include
  ${CMAKE_SOURCE_DIR}/programs/artiste/src
  ${CMAKE_SOURCE_DIR}/programs/artiste/src/vsx_widget
  ${CMAKE_SOURCE_DIR}/programs/artiste/include
)

################################################################################
# DEFINES ######################################################################
################################################################################

if(UNIX)
  add_definitions(-D_LINUX)
endif(UNIX)

if(VSXU_DEBUG)
  add_definitions( -DDEBUG )
  add_definitions( -DVSXU_DEBUG )
endif(VSXU_DEBUG)


add_definitions(
 -DVSXU_ARTISTE
 -DCMAKE_INSTALL_PREFIX="${CMAKE_INSTALL_PREFIX}"
)

if (VSXU_STATIC EQUAL 1)
  add_definitions(-DVSXU_STATIC)
  add_definitions(-DGLEW_STATIC)
  add_definitions(-DFTGL_LIBRARY_STATIC)
endif()



################################################################################
# SOURCES ######################################################################
################################################################################

set(SOURCES
  src/log/vsx_log_a.cpp
  src/vsx_widget/server/vsx_widget_comp.cpp
  src/vsx_widget/server/vsx_widget_connector_bezier.cpp
  src/vsx_widget/server/vsx_widget_anchor.cpp
  src/vsx_widget/server/vsx_widget_server.cpp
  src/vsx_widget/controllers/vsx_widget_controller_ab.cpp
  src/vsx_widget/controllers/vsx_widget_controller_base.cpp
  src/vsx_widget/controllers/vsx_widget_controller_channel.cpp
  src/vsx_widget/controllers/vsx_widget_controller_color.cpp
  src/vsx_widget/controllers/vsx_widget_controller_dialog.cpp
  src/vsx_widget/controllers/vsx_widget_controller_editor.cpp
  src/vsx_widget/controllers/vsx_widget_controller_knob.cpp
  src/vsx_widget/controllers/vsx_widget_controller_mixer.cpp
  src/vsx_widget/controllers/vsx_widget_controller_pad.cpp
  src/vsx_widget/controllers/vsx_widget_controller_seq.cpp
  src/vsx_widget/controllers/vsx_widget_controller_slider.cpp
  src/vsx_widget/helpers/vsx_widget_preview.cpp
  src/vsx_widget/helpers/vsx_widget_assistant.cpp
  src/vsx_widget/helpers/vsx_widget_object_inspector.cpp
  src/vsx_widget/helpers/vsx_widget_console.cpp
  src/vsx_widget/helpers/vsx_widget_note.cpp
  src/vsx_widget/sequencer/vsx_widget_master_sequencer.cpp
  src/vsx_widget/sequencer/vsx_widget_sequence_channel.cpp
  src/vsx_widget/sequencer/vsx_widget_sequence_editor.cpp
  src/vsx_widget/sequencer/pool_manager/vsx_widget_seq_pool.cpp
  src/vsx_widget/sequencer/vsx_widget_timeline.cpp
  src/vsx_widget/arcball/arcball.cpp
  src/vsx_widget/module_choosers/vsx_widget_module_chooser.cpp
  src/vsx_widget/module_choosers/vsx_widget_module_chooser_list.cpp
  src/artiste_main.cpp
)

file(GLOB_RECURSE HEADER_FILES *.h)


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
      vsx_widget
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
      vsx_widget
      vsx_common
      vsx_compression
      vsx_engine
      vsx_engine_graphics
    )
  endif()
endif(UNIX)


if(WIN32)
  target_link_libraries(
    ${module_id}
    vsx_application
    ${VSXU_GFX_LIB_LIBS}
    wsock32
    ws2_32
    vsx_engine_graphics
    vsx_engine
    vsx_common
    vsx_compression
    vsx_widget
    gdi32
    ${OPENGL_LIBRARIES}
    ${JPEG_LIBRARIES}
    ${PNG_LIBRARIES}
    ${ZLIB_LIBRARIES}
    -lstdc++
    ${CMAKE_THREAD_LIBS_INIT}
    ${FREETYPE_LIBRARIES}
  )
endif(WIN32)


################################################################################
## INSTALL #####################################################################
################################################################################

install(TARGETS ${module_id} DESTINATION ${VSXU_INSTALL_BIN_DIR} COMPONENT artiste)
install(
  DIRECTORY
    ${CMAKE_SOURCE_DIR}/share/visuals_faders
    ${CMAKE_SOURCE_DIR}/share/visuals_player
    ${CMAKE_SOURCE_DIR}/share/font
    ${CMAKE_SOURCE_DIR}/share/gfx/vsxu_luna
    ${CMAKE_SOURCE_DIR}/share/example-macros
    ${CMAKE_SOURCE_DIR}/share/example-prods
    ${CMAKE_SOURCE_DIR}/share/example-resources
    ${CMAKE_SOURCE_DIR}/share/example-states
    ${CMAKE_SOURCE_DIR}/share/example-faders
    ${CMAKE_SOURCE_DIR}/share/doc
    ${CMAKE_SOURCE_DIR}/share/example-visuals
  DESTINATION
    ${VSXU_INSTALL_SHARE_DIR}
  COMPONENT
    artiste
)

install(
  DIRECTORY
    ${CMAKE_SOURCE_DIR}/share/gfx/vsxu_luna
  DESTINATION
    ${VSXU_INSTALL_SHARE_DIR}/gfx
  COMPONENT
    artiste
)

install(
  FILES
    ${CMAKE_SOURCE_DIR}/share/gfx/vsxu_logo.dds
    ${CMAKE_SOURCE_DIR}/share/gfx/vsxu_logo_bkg.dds
    ${CMAKE_SOURCE_DIR}/share/gfx/assistant_luna.dds
    ${CMAKE_SOURCE_DIR}/share/vsxu.conf
  DESTINATION
    ${VSXU_INSTALL_SHARE_DIR}/gfx
  COMPONENT
    artiste
)

install(
  FILES
    ${CMAKE_SOURCE_DIR}/share/vsxu.conf
  DESTINATION
    ${VSXU_INSTALL_SHARE_DIR}
  COMPONENT
    artiste
)

IF (${CMAKE_SYSTEM_NAME} MATCHES "Linux")

  CONFIGURE_FILE (${CMAKE_CURRENT_SOURCE_DIR}/vsxu-artiste-fullscreen.desktop.in
                  ${CMAKE_CURRENT_BINARY_DIR}/vsxu-artiste-fullscreen.desktop
                  @ONLY)
  CONFIGURE_FILE (${CMAKE_CURRENT_SOURCE_DIR}/vsxu-artiste.desktop.in
                  ${CMAKE_CURRENT_BINARY_DIR}/vsxu-artiste.desktop
                  @ONLY)

  INSTALL (FILES ${CMAKE_CURRENT_BINARY_DIR}/vsxu-artiste-fullscreen.desktop
                 ${CMAKE_CURRENT_BINARY_DIR}/vsxu-artiste.desktop
           DESTINATION ${CMAKE_INSTALL_PREFIX}/share/applications/)
ENDIF (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
