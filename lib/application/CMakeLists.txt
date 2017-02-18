set(module_id vsx_application)

message("configuring            " ${module_id})

################################################################################
project (${module_id})

################################################################################
# CMAKE PACKAGES ###############################################################
################################################################################
include(${CMAKE_SOURCE_DIR}/cmake/CMakeFindLib.txt)
include(${CMAKE_SOURCE_DIR}/cmake/CMakeVSXuGfxLib.txt)


################################################################################
# INCLUDES #####################################################################
################################################################################

include_directories(
  ${CMAKE_SOURCE_DIR}/
  ${CMAKE_SOURCE_DIR}/lib/common/include
  ${CMAKE_SOURCE_DIR}/lib/engine_graphics/include
  ${CMAKE_SOURCE_DIR}/lib/application/include
)

file(GLOB_RECURSE HEADER_FILES *.h)

################################################################################
# DEFINES
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

if (VSXU_GFX_LIB EQUAL "GLFW")
    set(GFX_LIB_SOURCES
        src/glfw/vsx_mouse.cpp
    )
else()
    set(GFX_LIB_SOURCES
        src/sdl/vsx_application_input_mouse_control.cpp
        src/sdl/vsx_application_run.cpp
    )
endif()

set(SOURCES
  src/vsx_application_control.cpp
  src/vsx_application_manager.cpp
)

################################################################################
# LINKAGE ######################################################################
################################################################################

if (VSXU_STATIC EQUAL 1)
  add_library(${module_id} STATIC ${SOURCES} ${GFX_LIB_SOURCES} ${HEADER_FILES})
else ()
  add_library(${module_id} SHARED ${SOURCES} ${GFX_LIB_SOURCES} ${HEADER_FILES})
endif ()

install(TARGETS ${module_id} DESTINATION ${VSXU_INSTALL_LIB_DIR} COMPONENT common)

######################################################################################
# Install the Headers so as to install VSXu as a library
install (DIRECTORY "include/" DESTINATION ${CMAKE_INSTALL_PREFIX}/${VSXU_INSTALL_INCLUDE_DIR}/application COMPONENT Devel)
################################################################################

if (WIN32)
  if (VSXU_BUILD_WINDOWS_32)
    install(FILES  DESTINATION ${VSXU_INSTALL_BIN_DIR} COMPONENT common)
  endif()
endif()

add_subdirectory(test)
