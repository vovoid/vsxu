set(module_id vsxu_artiste)

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
  ${CMAKE_SOURCE_DIR}/engine_graphics/include
  ${CMAKE_SOURCE_DIR}/artiste/src
  ${CMAKE_SOURCE_DIR}/artiste/src/vsx_widget
  ${CMAKE_SOURCE_DIR}/artiste/src/vsx_widget/lib
  ${CMAKE_SOURCE_DIR}/artiste/include
)

################################################################################
# DEFINES ######################################################################
################################################################################

if(UNIX)
  add_definitions(-D_LINUX)
endif(UNIX)

if(VSXU_DEBUG)
  add_definitions(
  -DDEBUG
  )
endif(VSXU_DEBUG)


add_definitions(
 -DVSX_FONT_NO_FT
 -DVSXU_ARTISTE
 -DCMAKE_INSTALL_PREFIX="${CMAKE_INSTALL_PREFIX}"
)


################################################################################
# SOURCES ######################################################################
################################################################################

set(SOURCES
  src/log/vsx_log_a.cpp
  src/vsx_widget/vsx_widget_base.cpp
  src/vsx_widget/vsx_widget_camera.cpp
  src/vsx_widget/vsx_widget_time.cpp
  src/vsx_widget/vsx_widget_coords.cpp
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
  src/vsx_widget/window/vsx_widget_window.cpp
  src/vsx_widget/helpers/vsx_widget_preview.cpp
  src/vsx_widget/helpers/vsx_widget_assistant.cpp
  src/vsx_widget/helpers/vsx_widget_object_inspector.cpp
  src/vsx_widget/helpers/vsx_widget_console.cpp
  src/vsx_widget/helpers/vsx_widget_note.cpp
  src/vsx_widget/lib/vsx_widget_scrollbar.cpp
  src/vsx_widget/lib/vsx_widget_2d_label.cpp
  src/vsx_widget/lib/vsx_widget_2d_pager.cpp
  src/vsx_widget/lib/vsx_widget_button.cpp
  src/vsx_widget/lib/vsx_widget_popup_menu.cpp
  src/vsx_widget/lib/vsx_widget_panel.cpp
  src/vsx_widget/lib/vsx_widget_base_edit.cpp
  src/vsx_widget/dialogs/vsx_widget_window_statics.cpp
  src/vsx_widget/sequencer/vsx_widget_master_sequencer.cpp
  src/vsx_widget/sequencer/vsx_widget_seq_chan.cpp
  src/vsx_widget/sequencer/pool_manager/vsx_widget_seq_pool.cpp
  src/vsx_widget/sequencer/vsx_widget_sequence.cpp
  src/vsx_widget/sequencer/vsx_widget_timeline.cpp
  src/vsx_widget/profiler/vsx_widget_profiler.cpp
  src/vsx_widget/profiler/vsx_widget_profiler_timeline.cpp
  src/vsx_widget/arcball/arcball.cpp
  src/vsx_widget/module_choosers/vsx_widget_module_chooser.cpp
  src/vsx_widget/module_choosers/vsx_widget_module_chooser_list.cpp
  src/vsx_widget/vsx_widget_desktop.cpp
  src/application.cpp
  src/artiste_main.cpp
  src/vsx_mouse.cpp
)

################################################################################
# LINK #########################################################################
################################################################################

add_executable(${module_id} ${SOURCES} )

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
    ${TM_LIBRARY}
    vsxu_engine_graphics
    vsxu_engine
  )

  if (VSXU_ENGINE_STATIC EQUAL 1)
    include(../plugins/static_factory_cmake_target_link_libraries)
  endif()

endif(UNIX)


if(WIN32)
  target_link_libraries(
    ${module_id}
    ${GLFW_LIBRARY}
    wsock32
    ws2_32
    vsxu_engine
    vsxu_engine_graphics
    ${GLEW_LIBRARY}
    ${CMAKE_THREAD_LIBS_INIT}
    gdi32
    ${OPENGL_LIBRARIES}
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
    ${CMAKE_SOURCE_DIR}/share/gfx/vsxu_logo.jpg
    ${CMAKE_SOURCE_DIR}/share/gfx/vsxu_logo_bkg.jpg
    ${CMAKE_SOURCE_DIR}/share/gfx/luna.png
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
