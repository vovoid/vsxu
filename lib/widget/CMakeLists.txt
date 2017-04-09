set(module_id vsx_widget)

message("configuring            " ${module_id})

################################################################################
project (${module_id})


include(${CMAKE_SOURCE_DIR}/cmake/CMakeFindLib.txt)

################################################################################
# CMAKE PACKAGES ###############################################################
################################################################################
find_package(Threads REQUIRED)
find_package(OpenGL REQUIRED)

################################################################################
# INCLUDES #####################################################################
################################################################################

include_directories(
  ${CMAKE_SOURCE_DIR}/
  ${CMAKE_SOURCE_DIR}/lib/common/include
  ${CMAKE_SOURCE_DIR}/lib/application/include
  ${CMAKE_SOURCE_DIR}/lib/engine/include
  ${CMAKE_SOURCE_DIR}/lib/engine_graphics/include
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
)
if (MSVC)
else()
  add_definitions(
   -Werror=float-equal
  )
endif()

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
  src/vsx_widget_camera.cpp
  src/vsx_widget_coords.cpp
  src/vsx_widget_skin.cpp
  src/vsx_widget_time.cpp
  src/vsx_widget_window.cpp
  src/vsx_widget_global_interpolation.cpp
  src/widgets/vsx_widget_2d_pager.cpp
  src/widgets/vsx_widget_base_edit.cpp
  src/widgets/vsx_widget_panel.cpp
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


if (UNIX)
  install(TARGETS ${module_id} DESTINATION ${VSXU_INSTALL_LIB_DIR} COMPONENT widget)

  ######################################################################################
  # Install the Headers so as to install VSXu as a library
  install (DIRECTORY "include/" DESTINATION ${CMAKE_INSTALL_PREFIX}/${VSXU_INSTALL_INCLUDE_DIR}/widget COMPONENT Devel)
  ################################################################################
endif()

if(UNIX)
target_link_libraries(
  ${module_id}
  ${JPEG_LIBRARIES}
  ${PNG_LIBRARIES}
  ${ZLIB_LIBRARIES}
  ${GLFW_LIBRARIES}
  ${GLEW_LIBRARIES}
  vsx_engine
  ${OPENGL_LIBRARIES}
  ${CMAKE_THREAD_LIBS_INIT}
)
endif(UNIX)

if(WIN32)
  target_link_libraries(
    ${module_id}
    ${JPEG_LIBRARIES}
    ${PNG_LIBRARIES}
    ${ZLIB_LIBRARIES}
    ${GLFW_LIBRARIES}
    vsx_engine
    vsx_engine_graphics
    ${GLEW_LIBRARIES}
    ${OPENGL_LIBRARIES}
    vsx_engine_graphics
  )
endif(WIN32)

