cmake_minimum_required(VERSION 2.8)
if(POLICY CMP0040)
  cmake_policy(SET CMP0040 OLD)
endif()

PROJECT(VSXu)

set(VSXU_VERSION_MAJOR "0")
set(VSXU_VERSION_MINOR "6")
set(VSXU_VERSION_PATCH "2")
set(VSXU_VERSION ${VSXU_VERSION_MAJOR}.${VSXU_VERSION_MINOR}.${VSXU_VERSION_PATCH})

message("---------------------------------------------------------------------")
message("                     Configuring Vovoid VSXu                         ")
message("=====================================================================")
################################################################################
# CMAKE MODULE PATH ############################################################
################################################################################
set (CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules)

################################################################################
# GLOBAL INSTALL VARIABLES #####################################################
################################################################################

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    set(VSXU_INSTALL_INCLUDE_DIR "include/vsxu")
    set(VSXU_INSTALL_LIB_DIR "lib${LIB_SUFFIX}")
    set(VSXU_INSTALL_BIN_DIR "bin")
    set(VSXU_INSTALL_SHARE_DIR "share/vsxu")
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    if (VSXU_WINDOWS_LIB_BUILD)
      set(VSXU_INSTALL_INCLUDE_DIR "include/vsxu")
      set(VSXU_INSTALL_LIB_DIR "lib${LIB_SUFFIX}")
      set(VSXU_INSTALL_BIN_DIR "bin")
      set(VSXU_INSTALL_SHARE_DIR "share/vsxu")
    else()
      set(VSXU_INSTALL_INCLUDE_DIR "include")
      set(VSXU_INSTALL_LIB_DIR ".")
      set(VSXU_INSTALL_BIN_DIR ".")
      set(VSXU_INSTALL_SHARE_DIR "share")
    endif()
endif()

######################################################################################
enable_testing()

######################################################################################
# Install the Global Headers so as to install VSXu as a library
if (UNIX OR VSXU_WINDOWS_LIB_BUILD)
  file(GLOB HEADERS "*.h")
  install (FILES ${HEADERS} DESTINATION ${CMAKE_INSTALL_PREFIX}/${VSXU_INSTALL_INCLUDE_DIR} COMPONENT Devel)
endif()




################################################################################
# DEFINES ######################################################################
################################################################################

# C++14: set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive" )

if (MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -MP8")
  set(VSXU_STATIC 1)
  add_definitions(-DVSXU_STATIC)
  add_definitions(-DGLEW_STATIC)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
  include_directories( dependencies/SDL2_prebuilt_windows/include )
  include_directories( dependencies/glew-1.7.0/include )
  include_directories( lib/engine_graphics/thirdparty/ftgl)
  include_directories( lib/engine_graphics/thirdparty/ftgl/src)
  include_directories( lib/engine_graphics/thirdparty/freetype2/include )
  include_directories( lib/engine_graphics/thirdparty/freetype2/include/freetype )
  include_directories( lib/engine_graphics/thirdparty/freetype2/include/freetype/config )

else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11" )
    add_definitions( -Wall -Wno-sign-compare)
    add_definitions(-fmax-errors=500)

    if (VSXU_STATIC EQUAL 1)
      add_definitions(-g)
      add_definitions(-DVSXU_STATIC)
      add_definitions(-DGLEW_STATIC)
    endif()

    if(VSXU_DEBUG_SYMBOLS)
      add_definitions(-g)
    endif()
endif()

if (VSXU_STATIC_AUDIO_SINK EQUAL 1)
  add_definitions(-DVSXU_AUDIO_SINK=1)
endif()

if (VSX_PRINTF_TO_FILE EQUAL 1)
  add_definitions(-DVSX_PRINTF_TO_FILE)
endif()



include (cmake/optimization.txt)

add_definitions(-DVSXU_INSTALL_LIB_DIR="${VSXU_INSTALL_LIB_DIR}")
#add_definitions(-DVSX_VECTOR_TYPE_DOUBLE=1)



################################################################################
# VSXU LIBRARIES ###############################################################
################################################################################
add_subdirectory(lib/engine_graphics)

# order matters - need to add plugins before engine to create static glue ######
add_subdirectory(plugins)
################################################################################

add_subdirectory(lib/application)
add_subdirectory(lib/common)
add_subdirectory(lib/compression)
add_subdirectory(lib/engine)
add_subdirectory(lib/widget)

################################################################################
# VSXU IMPLEMENTATIONS/BINARIES ################################################
################################################################################
add_subdirectory(programs/launcher)
add_subdirectory(programs/artiste)
add_subdirectory(programs/player)
if (VSXU_DEMO_PLAYER)
  add_subdirectory(programs/player_demo)
endif()

add_subdirectory(programs/profiler)

if(UNIX)
 add_subdirectory(programs/server)

endif(UNIX)

add_subdirectory(programs/tools/vsxbt)
add_subdirectory(programs/tools/vsxz)
add_subdirectory(programs/tools/obj2vxm)
#add_subdirectory(programs/tools/raw2wav)
if(UNIX)
    add_subdirectory(programs/tools/vsxl)
endif(UNIX)

################################################################################
# The pkg-config file
################################################################################
IF (UNIX OR VSXU_WINDOWS_LIB_BUILD)
  if (VSXU_STATIC EQUAL 1 AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/plugins/static_factory_pkgconfig_libs")
    file (STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/plugins/static_factory_pkgconfig_libs" VSXU_PLUGIN_PKGCONFIG_LIBS)
    CONFIGURE_FILE (${CMAKE_CURRENT_SOURCE_DIR}/libvsx.pc.in.static
                    ${CMAKE_CURRENT_BINARY_DIR}/libvsx.pc
                    @ONLY)

  else()
    CONFIGURE_FILE (${CMAKE_CURRENT_SOURCE_DIR}/libvsx.pc.in.dynamic
                    ${CMAKE_CURRENT_BINARY_DIR}/libvsx.pc
                    @ONLY)
  endif()

  INSTALL (FILES ${CMAKE_CURRENT_BINARY_DIR}/libvsx.pc
           DESTINATION ${VSXU_INSTALL_LIB_DIR}/pkgconfig)
ENDIF()



include(CPackLists.txt)

# uninstall target
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)

