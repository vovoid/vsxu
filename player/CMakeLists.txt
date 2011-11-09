set(module_id vsxu_player)
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
  ${CMAKE_SOURCE_DIR}/player/src
)

################################################################################
# DEFINES ######################################################################
################################################################################
add_definitions(
 -DVSX_FONT_NO_FT
 -DDEBUG
 -DVSX_TEXTURE_NO_RT
 -DVSXU_EXE
 -DVSXU_DEVELOPER
 -DVSXU_ARTISTE
 -DCMAKE_INSTALL_PREFIX="${CMAKE_INSTALL_PREFIX}"
)

if (VSXU_DEBUG EQUAL 1)
  add_definitions( -DVSXU_DEBUG )
endif (VSXU_DEBUG EQUAL 1)

################################################################################
# SOURCES ######################################################################
################################################################################

set(OS_SOURCES "")

if(WIN32)
  set(OS_SOURCES
    src/main/win32/main.cpp
    src/main/win32/includes/nehegl.cpp
  )
endif(WIN32)

if(UNIX)
  set(OS_SOURCES
    src/main/linux/main.cpp
  )
endif(UNIX)



set(SOURCES
  src/application.cpp
  src/vsx_overlay.cpp
)


add_executable(${module_id}  ${SOURCES} ${OS_SOURCES})
include(../cmake_suffix.txt)

if(UNIX)
  include(CMake_targets_install_unix.txt)
endif(UNIX)

if(WIN32)
  include(CMake_targets_install_windows.txt)
endif(WIN32)
