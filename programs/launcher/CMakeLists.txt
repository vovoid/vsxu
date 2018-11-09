set(module_id vsxu_launcher)

message("configuring            " ${module_id})


################################################################################
project (${module_id})

include(${CMAKE_SOURCE_DIR}/cmake/CMakeFindLib.txt)
include(${CMAKE_SOURCE_DIR}/cmake/CMakeVSXuGfxLib.txt)

################################################################################
# CMAKE PACKAGES ###############################################################
################################################################################
find_package(OpenGL REQUIRED)


################################################################################
# INCLUDES #####################################################################
################################################################################

include_directories(
  ${OPENGL_INCLUDE_DIR}
  ${CMAKE_CURRENT_LIST_DIR}/src
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
  src/launcher_main.cpp
)

file(GLOB GLOB_RESULT src/*.cpp)
set(SOURCES ${SOURCES} ${GLOB_RESULT})

# Solve the problem with DPI scaling
# since we want our own rc file
if(WIN32)
    set(CMAKE_EXE_LINKER_FLAGS "/MANIFEST:NO")
    set(SOURCES ${SOURCES} launcher.rc)
endif()

file(GLOB_RECURSE HEADER_FILES *.h)


set(AUDIO_LIBRARIES "")
set(WITH_MIDI FALSE)

if (WIN32)
    add_definitions(
      -D__WINDOWS_DS__
      -D_MBCS
    )
endif()

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  find_package(PULSEAUDIO)
  if(PULSEAUDIO_FOUND)
    set( DEFINITIONS -D__LINUX_PULSE__)
    set( INCLUDE_DIRS ${PULSEAUDIO_INCLUDE_DIRS})
    set( AUDIO_LIBRARIES ${PULSEAUDIO_LIBRARY} ${PULSEAUDIO_SIMPLE_LIBRARY})
  endif(PULSEAUDIO_FOUND)

  find_package(JACK)
  if(JACK_FOUND)
    set( DEFINITIONS ${DEFINITIONS} -D__LINUX_JACK__)
    set( WITH_MIDI TRUE )
    set( INCLUDE_DIRS ${INCLUDE_DIRS} ${JACK_INCLUDE_DIRS})
    set( AUDIO_LIBRARIES ${AUDIO_LIBRARIES} ${JACK_LIBRARY} )
  endif(JACK_FOUND)

  find_package(ALSA)
  if(ALSA_FOUND)
    set( DEFINITIONS ${DEFINITIONS} -D__LINUX_ALSA__)
    set( DEFINITIONS ${DEFINITIONS} -DWITH_MIDI )
    set( WITH_MIDI TRUE )
    set( INCLUDE_DIRS ${INCLUDE_DIRS} ${ALSA_INCLUDE_DIRS})
    set( AUDIO_LIBRARIES ${AUDIO_LIBRARIES} ${ALSA_LIBRARY} )
  endif(ALSA_FOUND)
endif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")


if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  find_library(COREAUDIO_LIBRARY CoreAudio)
  if(COREAUDIO_LIBRARY_FOUND)
    set( DEFINITIONS -D__MACOSX_CORE__)
    set( DEFINITIONS -D__OS_MACOSX__)
    set( AUDIO_LIBRARIES ${AUDIO_LIBRARIES} ${COREAUDIO_LIBRARY})
  endif(COREAUDIO_LIBRARY_FOUND)
endif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")


if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  set( DEFINITIONS -D__WINDOWS_DS__ -D__WINDOWS_MM__ -D__WINDOWS_WASAPI__=1)
  set( AUDIO_LIBRARIES ${AUDIO_LIBRARIES} winmm dsound)
endif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")

message(${AUDIO_LIBRARIES})
################################################################################
# LINK #########################################################################
################################################################################

add_executable(${module_id} ${SOURCES} ${HEADER_FILES} )
add_definitions(${DEFINITIONS})
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

install(TARGETS ${module_id} DESTINATION ${VSXU_INSTALL_BIN_DIR} COMPONENT launcher)

######################################################################################
# Install the Headers so as to install VSXu as a library
if (UNIX OR VSXU_WINDOWS_LIB_BUILD)
  file(GLOB HEADERS "include/*.h")
  install (FILES ${HEADERS} DESTINATION ${CMAKE_INSTALL_PREFIX}/${VSXU_INSTALL_INCLUDE_DIR}/profiler COMPONENT Devel)
endif()
################################################################################

