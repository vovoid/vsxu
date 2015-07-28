set(module_id vsx_widget)

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
  ${GLEW_INCLUDE_PATH}
  ../
  ../engine/include
  ../engine_graphics/include
  ../profiler/include
  include
  include/widgets
)



################################################################################
# DEFINES
################################################################################
# - if you want an engine without OpenGL linkage, add -DVSX_NO_GL
################################################################################

add_definitions(
 -DCMAKE_INSTALL_PREFIX="${CMAKE_INSTALL_PREFIX}"
 # -DBUILDING_DLL=1
)

if (VSXU_DEBUG EQUAL 1)
  add_definitions( -DVSXU_DEBUG )
endif (VSXU_DEBUG EQUAL 1)

if (VSXU_STATIC EQUAL 1)
  add_definitions(-DVSXU_STATIC)
  add_definitions(-DGLEW_STATIC)
endif()


################################################################################
# SOURCES ######################################################################
################################################################################

set(SOURCES
  src/vsx_widget.cpp
  src/vsx_mouse.cpp
  src/vsx_widget_camera.cpp
  src/vsx_widget_coords.cpp
  src/vsx_widget_skin.cpp
  src/vsx_widget_time.cpp
  src/vsx_widget_window.cpp
  src/vsx_widget_global_interpolation.cpp
  src/widgets/vsx_widget_2d_label.cpp
  src/widgets/vsx_widget_2d_pager.cpp
  src/widgets/vsx_widget_base_edit.cpp
  src/widgets/vsx_widget_button.cpp
  src/widgets/vsx_widget_panel.cpp
  src/widgets/vsx_widget_popup_menu.cpp
  src/widgets/vsx_widget_scrollbar.cpp
)
file(GLOB_RECURSE HEADER_FILES *.h)

################################################################################
# LINKAGE ######################################################################
################################################################################

if (VSXU_STATIC EQUAL 1)

  #static build
  add_library(${module_id} STATIC ${SOURCES} ${HEADER_FILES})

else ()

  #dynamic build
  add_library(${module_id} SHARED ${SOURCES} ${HEADER_FILES})

endif ()

install(TARGETS ${module_id} DESTINATION ${VSXU_INSTALL_LIB_DIR} COMPONENT widget)

######################################################################################
# Install the Headers so as to install VSXu as a library
if (UNIX OR VSXU_WINDOWS_LIB_BUILD)
  file(GLOB HEADERS "include/*.h")
  install (FILES ${HEADERS} DESTINATION ${CMAKE_INSTALL_PREFIX}/${VSXU_INSTALL_INCLUDE_DIR}/widget COMPONENT Devel)
endif()
################################################################################

if(UNIX)
target_link_libraries(
  ${module_id}
  ${JPEG_LIBRARY}
  ${PNG_LIBRARY}
  ${ZLIB_LIBRARY}
  ${GLFW_LIBRARY}
  ${GLEW_LIBRARY}
  vsxu_engine
  ${OPENGL_LIBRARIES}
  ${CMAKE_THREAD_LIBS_INIT}
)
endif(UNIX)

if(WIN32)
  target_link_libraries(
    ${module_id}
    ${JPEG_LIBRARY}
    ${PNG_LIBRARY}
    ${ZLIB_LIBRARY}
    ${GLFW_LIBRARY}
    vsxu_engine
    vsxu_engine_graphics
    ${GLEW_LIBRARY}
    ${OPENGL_LIBRARIES}
    vsxu_engine_graphics
  )
endif(WIN32)

