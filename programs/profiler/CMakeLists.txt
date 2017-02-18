set(module_id vsxu_profiler)

message("configuring            " ${module_id})


################################################################################
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
  ${OPENGL_INCLUDE_DIR}
  ${CMAKE_SOURCE_DIR}/
  ${CMAKE_SOURCE_DIR}/lib/application/include
  ${CMAKE_SOURCE_DIR}/lib/common/include
  ${CMAKE_SOURCE_DIR}/lib/engine/include
  ${CMAKE_SOURCE_DIR}/lib/engine_graphics/include
  ${CMAKE_SOURCE_DIR}/lib/widget/include
  ${CMAKE_SOURCE_DIR}/programs/profiler/src
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
 -DCMAKE_INSTALL_PREFIX="${CMAKE_INSTALL_PREFIX}"
)


################################################################################
# SOURCES ######################################################################
################################################################################

set(SOURCES
  src/profiler_main.cpp
  src/profiler/vsx_widget_profiler.cpp
  src/profiler/vsx_widget_profiler_timeline.cpp
  src/profiler/vsx_widget_profiler_thread.cpp
  src/profiler/vsx_widget_profiler_plot.cpp
)

file(GLOB_RECURSE HEADER_FILES *.h)


################################################################################
# LINK #########################################################################
################################################################################

add_executable(${module_id} ${SOURCES} ${HEADER_FILES} )

include(${CMAKE_SOURCE_DIR}/cmake_suffix.txt)

if(UNIX)
  target_link_libraries(
    ${module_id}
    ${CMAKE_THREAD_LIBS_INIT}
    ${GLEW_LIBRARIES}
    ${OPENGL_LIBRARIES}
    ${PNG_LIBRARIES}
    ${JPEG_LIBRARIES}
    vsx_common
    vsx_compression
    vsx_engine
    vsx_engine_graphics
    vsx_widget
    vsx_engine_graphics
    vsx_application
    ${VSXU_GFX_LIB_LIBS}
    rt
  )
endif(UNIX)

if (WIN32)
  target_link_libraries(
    ${module_id}
    ${CMAKE_THREAD_LIBS_INIT}
    ${GLEW_LIBRARIES}
    ${PNG_LIBRARIES}
    ${JPEG_LIBRARIES}
    vsx_application
    vsx_common
    vsx_compression
    vsx_engine
    vsx_engine_graphics
    vsx_widget
    vsx_application
    ${VSXU_GFX_LIB_LIBS}
  )

endif()

################################################################################
## INSTALL #####################################################################
################################################################################

install(TARGETS ${module_id} DESTINATION ${VSXU_INSTALL_BIN_DIR} COMPONENT profiler)

######################################################################################
# Install the Headers so as to install VSXu as a library
if (UNIX OR VSXU_WINDOWS_LIB_BUILD)
  file(GLOB HEADERS "include/*.h")
  install (FILES ${HEADERS} DESTINATION ${CMAKE_INSTALL_PREFIX}/${VSXU_INSTALL_INCLUDE_DIR}/profiler COMPONENT Devel)
endif()
################################################################################

add_subdirectory(test)
