cmake_minimum_required(VERSION 2.6)

get_filename_component(list_file_path ${CMAKE_CURRENT_LIST_FILE} PATH)
string(REGEX MATCH "[a-z._-]*$" module_id ${list_file_path})
message("configuring vsxu module            " ${module_id})


set(AUDIO_LIBRARIES "")
set(WITH_MIDI FALSE)

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  find_package(PULSEAUDIO)
  if(PULSEAUDIO_FOUND)
    add_definitions(-D__LINUX_PULSE__)

    include_directories(${PULSEAUDIO_INCLUDE_DIRS})
    set( AUDIO_LIBRARIES ${PULSEAUDIO_LIBRARY} ${PULSEAUDIO_SIMPLE_LIBRARY})
  endif(PULSEAUDIO_FOUND)

  find_package(JACK)
  if(JACK_FOUND)
    add_definitions(-D__LINUX_JACK__)
    set( WITH_MIDI TRUE )

    include_directories(${JACK_INCLUDE_DIRS})
    set( AUDIO_LIBRARIES ${AUDIO_LIBRARIES} ${JACK_LIBRARY} )
  endif(JACK_FOUND)

  find_package(ALSA)
  if(ALSA_FOUND)
    add_definitions(-D__LINUX_ALSA__)
    add_definitions( -DWITH_MIDI )
    set( WITH_MIDI TRUE )

    include_directories(${ALSA_INCLUDE_DIRS})
    set( AUDIO_LIBRARIES ${AUDIO_LIBRARIES} ${ALSA_LIBRARY} )

  endif(ALSA_FOUND)
endif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")


if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  find_library(COREAUDIO_LIBRARY CoreAudio)
  if(COREAUDIO_LIBRARY_FOUND)
    add_definitions(-D__MACOSX_CORE__)
    add_definitions(-D__OS_MACOSX__)

    set( AUDIO_LIBRARIES ${AUDIO_LIBRARIES} ${COREAUDIO_LIBRARY})
  endif(COREAUDIO_LIBRARY_FOUND)
endif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")


if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  add_definitions(-D__WINDOWS_DS__)
  set( AUDIO_LIBRARIES ${AUDIO_LIBRARIES} winmm dsound)
endif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")

#TODO: Add in the needed definitions for each Audio/Midi Library on each platform
if(AUDIO_LIBRARIES)
  project (${module_id})
  find_package(Threads)
  include (../cmake_globals.txt)
  include_directories ( lib/
                        fftreal/ )
  set(SOURCES main_sound.rtaudio.cpp
              fftreal/fftreal.cpp
              lib/RtAudio/RtAudio.cpp
              lib/RtMidi/RtMidi.cpp
)

  #if(WITH_MIDI)
#    list(APPEND SOURCES input/input_audio_midi.cpp)
#  endif(WITH_MIDI)

  add_library(${module_id} MODULE ${SOURCES})
  include(../cmake_suffix.txt)
  target_link_libraries(${module_id} ${AUDIO_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
endif(AUDIO_LIBRARIES)

