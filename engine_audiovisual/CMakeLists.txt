set(module_id vsxu_engine_audiovisual)

message("configuring            " ${module_id})


project (${module_id})

include(${CMAKE_SOURCE_DIR}/cmake/CMakeFindLib.txt)

################################################################################
# CMAKE PACKAGES ###############################################################
################################################################################
find_package(OpenGL REQUIRED)

################################################################################
# INCLUDES #####################################################################
################################################################################

include_directories(
  ${OPENGL_INCLUDE_DIR}
  ${CMAKE_SOURCE_DIR}/
  ${CMAKE_SOURCE_DIR}/engine_audiovisual/include
  ${CMAKE_SOURCE_DIR}/engine_graphics/include
  ${CMAKE_SOURCE_DIR}/engine/include
  ${CMAKE_SOURCE_DIR}/profiler/include
)


################################################################################
# DEFINITIONS ##################################################################
################################################################################

add_definitions(
   # -DVSX_AUDOVISUAL_DLL
 -DVSXU_DEVELOPER
 -DCMAKE_INSTALL_PREFIX="${CMAKE_INSTALL_PREFIX}"
)

if (VSXU_DEBUG EQUAL 1)
  add_definitions( -DVSXU_DEBUG -DDEBUG )
endif (VSXU_DEBUG EQUAL 1)

if (VSXU_STATIC EQUAL 1)
  add_definitions(-DGLEW_STATIC=1)
  add_definitions(-DVSXU_STATIC)
endif()


################################################################################
# SOURCES ######################################################################
################################################################################

set(SOURCES
  src/vsx_statelist.cpp
  src/vsx_manager.cpp
)

if (VSXU_STATIC EQUAL 1)
  add_library(${module_id} STATIC ${SOURCES})
else()
  add_library(${module_id} SHARED ${SOURCES})
endif()

install(TARGETS ${module_id} DESTINATION ${VSXU_INSTALL_LIB_DIR} COMPONENT engine)

######################################################################################
# Install the Headers so as to install VSXu as a library
if (UNIX OR VSXU_WINDOWS_LIB_BUILD)
  file(GLOB HEADERS "include/*.h")
  install (FILES ${HEADERS} DESTINATION ${CMAKE_INSTALL_PREFIX}/${VSXU_INSTALL_INCLUDE_DIR}/engine_audiovisual COMPONENT Devel)
endif()
################################################################################

target_link_libraries(${module_id}
  vsxu_engine
  vsxu_engine_graphics
  ${OPENGL_LIBRARIES}
)

if (WIN32)
  target_link_libraries(${module_id} gdi32)
endif()
