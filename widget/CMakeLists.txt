set(module_id vsx_widget)

message("configuring            " ${module_id})

################################################################################
project (${module_id})


################################################################################
# CMAKE PACKAGES ###############################################################
################################################################################
find_package(Freetype REQUIRED)


################################################################################
# INCLUDES #####################################################################
################################################################################

include_directories(
  ${GLEW_INCLUDE_PATH}
  ../
  ../engine/include
  ../engine_graphics/include
  include
  include/widgets
)

include_directories(${FREETYPE_INCLUDE_DIRS})


################################################################################
# DEFINES
################################################################################
# - if you want an engine without OpenGL linkage, add -DVSX_NO_GL
################################################################################

add_definitions(
 -DCMAKE_INSTALL_PREFIX="${CMAKE_INSTALL_PREFIX}"
) 

if (VSXU_DEBUG EQUAL 1)
  add_definitions( -DVSXU_DEBUG )
endif (VSXU_DEBUG EQUAL 1)


################################################################################
# SOURCES ######################################################################
################################################################################

set(SOURCES
  src/vsx_widget.cpp
  src/vsx_mouse.cpp
  src/vsx_widget_camera.cpp
  src/vsx_widget_coords.cpp
  src/vsx_widget_time.cpp
  src/vsx_widget_window.cpp
  src/widgets/vsx_widget_2d_label.cpp
  src/widgets/vsx_widget_2d_pager.cpp
  src/widgets/vsx_widget_base_edit.cpp
  src/widgets/vsx_widget_button.cpp
  src/widgets/vsx_widget_panel.cpp
  src/widgets/vsx_widget_popup_menu.cpp
  src/widgets/vsx_widget_scrollbar.cpp
)

################################################################################
# LINKAGE ######################################################################
################################################################################

if (VSXU_STATIC EQUAL 1)

  #static build
  add_library(${module_id} STATIC ${SOURCES} ${GAME_MONKEY})

else ()

  #dynamic build
  add_library(${module_id} SHARED ${SOURCES} ${GAME_MONKEY})

endif ()

install(TARGETS ${module_id} DESTINATION ${VSXU_INSTALL_LIB_DIR} COMPONENT widget)

######################################################################################
# Install the Headers so as to install VSXu as a library
if (UNIX)
  file(GLOB HEADERS "include/*.h")
  install (FILES ${HEADERS} DESTINATION ${CMAKE_INSTALL_PREFIX}/${VSXU_INSTALL_INCLUDE_DIR}/widget COMPONENT Devel)
endif()
################################################################################
