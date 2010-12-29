cmake_minimum_required(VERSION 2.6)
include(../cmake_globals.txt)
include_directories(
  ../
  ../engine/include
  ../engine_graphics/include
  src
  src/vsx_font
  src/vsx_widget
  include
)

set(OS_SOURCES "")

if(WIN32)
  include_directories(src/main/win32/)
  set(OS_SOURCES
    src/main/win32/main.cpp
	src/main/win32/vsx_mouse.cpp
    src/main/win32/includes/nehegl.cpp
  )
  include_directories("../../_system_win32" "../../_system_win32/pthread" "../../_lib/win32")
endif(WIN32)

if(UNIX)
  add_definitions(-D_LINUX)
  include_directories(src/main/linux/)
  set(OS_SOURCES
    src/main/linux/main.cpp
    src/main/linux/vsx_mouse.cpp
  )
endif(UNIX)


if(VSXU_DEBUG)
add_definitions(
 -DDEBUG
)
endif(VSXU_DEBUG)

if (PLATFORM_SHARED_FILES_FLAT_INSTALL EQUAL 1)
  add_definitions( -DPLATFORM_SHARED_FILES_FLAT_INSTALL )
endif (PLATFORM_SHARED_FILES_FLAT_INSTALL EQUAL 1)


#definitions
add_definitions(
 -DVSX_TEXTURE_NO_RT
 -DVSX_FONT_NO_FT
 -DVSXU_EXE
 -DVSXU_ARTISTE
 -DCMAKE_INSTALL_PREFIX="${CMAKE_INSTALL_PREFIX}"
)

get_filename_component(list_file_path ${CMAKE_CURRENT_LIST_FILE} PATH)
string(REGEX MATCH "[a-z._-]*$" module_id ${list_file_path})
message("*******************************************")
message("configuring            " ${module_id})
message("*******************************************")

set(SOURCES
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
  src/vsx_widget/vsx_widget_main.cpp
  src/application.cpp
  src/gl_helper.cpp
)

link_directories(
../
)



project (${module_id})

add_executable(${module_id}  ${SOURCES} ${OS_SOURCES} ${EXTRA_LIBS})
include(../cmake_suffix.txt)




set_target_properties(
  ${module_id}
    PROPERTIES
      OUTPUT_NAME
        vsxu_artiste
)

if(UNIX)
  target_link_libraries(${module_id}
    Xrandr
    X11
    glfw
    pthread
    GLEW
    GL
    GLU
    png
    jpeg
    vsxu_engine_graphics
    vsxu_engine
  )
  install(TARGETS ${module_id} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
  install(
    DIRECTORY
      ../share/visuals_faders
      ../share/visuals_player
      ../share/font
      ../share/gfx/vsxu_luna
      ../share/example-macros
      ../share/example-prods
      ../share/example-resources
      ../share/example-states
      ../share/example-faders
      ../share/doc
      ../share/example-visuals
    DESTINATION
      ${CMAKE_INSTALL_PREFIX}/share/vsxu)
  install(
    DIRECTORY
      ../share/gfx/vsxu_luna
    DESTINATION
      ${CMAKE_INSTALL_PREFIX}/share/vsxu/gfx)
  install(
    FILES
      ../share/gfx/vsxu_logo.jpg
      ../share/gfx/vsxu_logo_bkg.jpg
      ../share/gfx/luna.png
      ../share/vsxu.conf
    DESTINATION
      ${CMAKE_INSTALL_PREFIX}/share/vsxu/gfx/
  )
  install(
    FILES
      ../share/vsxu.conf
    DESTINATION
      ${CMAKE_INSTALL_PREFIX}/share/vsxu/
  )
  if (VSXU_DEBUG EQUAL 1)
  install(
    DIRECTORY
      ../share/debug-states
    DESTINATION
      ${CMAKE_INSTALL_PREFIX}/share/vsxu)
  endif (VSXU_DEBUG EQUAL 1)
endif(UNIX)


if(WIN32)
  link_directories("../_lib/win32" ../engine_graphics)

  target_link_libraries(${module_id}
    wsock32
    ws2_32
    vsxu_engine
	vsxu_engine_graphics
    glew
    pthreadGCE
    gdi32
    glu32
    opengl32
  )
endif(WIN32)
