set(module_id vsxu_profiler)

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
  ${CMAKE_SOURCE_DIR}/widget/include
  ${CMAKE_SOURCE_DIR}/profiler/src
  ${CMAKE_SOURCE_DIR}/profiler/include
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
 -DCMAKE_INSTALL_PREFIX="${CMAKE_INSTALL_PREFIX}"
)


################################################################################
# SOURCES ######################################################################
################################################################################

set(SOURCES
  src/profiler_main.cpp
  src/profiler_application.cpp
  src/profiler_desktop.cpp

  src/profiler/vsx_widget_profiler.cpp
  src/profiler/vsx_widget_profiler_timeline.cpp
  src/profiler/vsx_widget_profiler_thread.cpp
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
    vsxu_engine
    vsxu_engine_graphics
    vsx_widget
    rt
  )

endif(UNIX)

################################################################################
## INSTALL #####################################################################
################################################################################

install(TARGETS ${module_id} DESTINATION ${VSXU_INSTALL_BIN_DIR} COMPONENT profiler)

######################################################################################
# Install the Headers so as to install VSXu as a library
if (UNIX)
  file(GLOB HEADERS "include/*.h")
  install (FILES ${HEADERS} DESTINATION ${CMAKE_INSTALL_PREFIX}/${VSXU_INSTALL_INCLUDE_DIR}/profiler COMPONENT Devel)
endif()
################################################################################
