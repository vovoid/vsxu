cmake_minimum_required(VERSION 2.6)

get_filename_component(list_file_path ${CMAKE_CURRENT_LIST_FILE} PATH)
string(REGEX MATCH "[a-z0-9._-]*$" module_id ${list_file_path})

message("configuring vsxu module            " ${module_id})


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
  set( DEFINITIONS -D__WINDOWS_DS__)
  set( AUDIO_LIBRARIES ${AUDIO_LIBRARIES} winmm dsound)
endif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")

message(${AUDIO_LIBRARIES})

#TODO: Add in the needed definitions for each Audio/Midi Library on each platform
if(AUDIO_LIBRARIES)
  project (${module_id})
  find_package(Threads)
  include (../cmake_globals.txt)
  set(INCLUDE_DIRS
    ${INCLUDE_DIRS}
    ${CMAKE_CURRENT_LIST_DIR}/lib/
    ${CMAKE_CURRENT_LIST_DIR}/fftreal/
  )
  include_directories (${INCLUDE_DIRS})
  add_definitions(${DEFINITIONS})

message("audio libraries:\n")
message(${AUDIO_LIBRARIES})
  # set defines for MOD_CM, MOD_DM, MOD_NM
  CREATE_STATIC_GLUE(${module_id})

  set(SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/main_sound.rtaudio.cpp
    ${CMAKE_CURRENT_LIST_DIR}/fftreal/fftreal.cpp
    ${CMAKE_CURRENT_LIST_DIR}/lib/RtAudio/RtAudio.cpp
    ${CMAKE_CURRENT_LIST_DIR}/lib/RtMidi/RtMidi.cpp
  )
  file(GLOB_RECURSE HEADER_FILES *.h)

  #if(WITH_MIDI)
#    list(APPEND SOURCES input/input_audio_midi.cpp)
#  endif(WITH_MIDI)

  # actual compilation target
  CREATE_MODULE(${module_id})

  include(../cmake_suffix.txt)

  if(NOT (VSXU_STATIC EQUAL 1))
    target_link_libraries(${module_id} ${AUDIO_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
  endif()

#  set(LIBRARIES ${AUDIO_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

endif(AUDIO_LIBRARIES)

