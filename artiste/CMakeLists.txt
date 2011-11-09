set(module_id vsxu_artiste)
message("*******************************************")
message("configuring            " ${module_id})
message("*******************************************")

################################################################################
project (${module_id})

################################################################################
# CMAKE PACKAGES ###############################################################
################################################################################
include(cmake_packages.txt)

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
  # -DVSXU_DEBUG
  # -DVSX_DEBUG
  )
endif(VSXU_DEBUG)


add_definitions(
 -DVSX_TEXTURE_NO_RT
 -DVSX_FONT_NO_FT
 -DVSXU_EXE
 -DVSXU_ARTISTE
 -DCMAKE_INSTALL_PREFIX="${CMAKE_INSTALL_PREFIX}"
)

################################################################################
# SOURCES ######################################################################
################################################################################

set(OS_SOURCES "")

if(WIN32)
  include_directories(src/main/win32/)
  set(OS_SOURCES
    src/main/win32/main.cpp
    src/main/win32/vsx_mouse.cpp
    src/main/win32/includes/nehegl.cpp
  )
endif(WIN32)

if(UNIX)
  include_directories(src/main/linux/)
  set(OS_SOURCES
    src/main/linux/main.cpp
    src/main/linux/vsx_mouse.cpp
  )
endif(UNIX)

set(SOURCES
  src/gl_helper.cpp
  src/log/vsx_log_a.cpp
  src/vsx_widget/vsx_widget_base.cpp
  src/vsx_widget/server/vsx_widget_comp.cpp
  src/vsx_widget/server/vsx_widget_connector.cpp
  src/vsx_widget/server/vsx_widget_anchor.cpp
  src/vsx_widget/server/vsx_widget_server.cpp
  src/vsx_widget/controllers/vsx_widget_controller.cpp
  src/vsx_widget/controllers/vsx_widget_base_controller.cpp
  src/vsx_widget/controllers/vsx_widget_controller_ab.cpp
  src/vsx_widget/controllers/vsx_widget_editor.cpp
  src/vsx_widget/controllers/vsx_widget_controller_seq.cpp
  src/vsx_widget/window/vsx_widget_window.cpp
  src/vsx_widget/helpers/vsx_widget_preview.cpp
  src/vsx_widget/helpers/vsx_widget_assistant.cpp
  src/vsx_widget/helpers/vsx_widget_object_inspector.cpp
  src/vsx_widget/helpers/vsx_widget_console.cpp
  src/vsx_widget/helpers/vsx_widget_note.cpp
  src/vsx_widget/lib/vsx_widget_scrollbar.cpp
  src/vsx_widget/lib/vsx_widget_lib.cpp
  src/vsx_widget/lib/vsx_widget_panel.cpp
  src/vsx_widget/lib/vsx_widget_base_edit.cpp
  src/vsx_widget/dialogs/vsx_widget_window_statics.cpp
  src/vsx_widget/sequencer/vsx_widget_master_sequencer.cpp
  src/vsx_widget/sequencer/vsx_widget_seq_chan.cpp
  src/vsx_widget/sequencer/pool_manager/vsx_widget_seq_pool.cpp
  src/vsx_widget/sequencer/vsx_widget_sequence.cpp
  src/vsx_widget/sequencer/vsx_widget_timeline.cpp
  src/vsx_widget/arcball/arcball.cpp
  src/vsx_widget/module_choosers/vsx_widget_module_chooser.cpp
  src/vsx_widget/module_choosers/vsx_widget_module_chooser_list.cpp
  src/vsx_widget/vsx_widget_desktop.cpp
  src/application.cpp
)

################################################################################
# LINKAGE ######################################################################
################################################################################

add_executable(${module_id} ${SOURCES} ${OS_SOURCES})

include(../cmake_suffix.txt)

if(UNIX)
  include(CMake_targets_install_unix.txt)
endif(UNIX)


if(WIN32)
  include(CMake_targets_install_windows.txt)
endif(WIN32)
