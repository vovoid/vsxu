cmake_minimum_required(VERSION 2.6)

get_filename_component(list_file_path ${CMAKE_CURRENT_LIST_FILE} PATH)
string(REGEX MATCH "[a-z._-]*$" module_id ${list_file_path})
message("configuring vsxu module            " ${module_id})


set(AUDIO_LIBRARIES "")

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  find_package(PULSEAUDIO)
  if(PULSEAUDIO_FOUND)
    if(NOT USE_ALSA) #Do not use pulseaudio if one explicitly prefers alsa
      add_definitions(-D__LINUX_PULSE__)

      include_directories(${PULSEAUDIO_INCLUDE_DIRS})
      set( AUDIO_LIBRARIES ${PULSEAUDIO_LIBRARY} ${PULSEAUDIO_SIMPLE_LIBRARY})
    endif(NOT USE_ALSA)
  endif(PULSEAUDIO_FOUND)
endif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  find_package(ALSA)
  if(ALSA_FOUND)
    if(NOT PULSEAUDIO_FOUND OR USE_ALSA) #If pulseaudio is not found or even if pulseaudio is found, if one prefers alsa
      add_definitions(-D__LINUX_ALSA__)

      include_directories(${ALSA_INCLUDE_DIRS})
      set( AUDIO_LIBRARIES ${AUDIO_LIBRARIES} ${ALSA_LIBRARY} )
    endif(NOT PULSEAUDIO_FOUND OR USE_ALSA)
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

#TODO: Add in the needed definitions for each Audio Library on each platform
if(AUDIO_LIBRARIES)
  project (${module_id})
  find_package(PTHREAD)
  include (../cmake_globals.txt)
  include_directories ( lib/
                        lib/fftreal/ )
  set(SOURCES main.cpp
              input/input_audio_raw.cpp
              input/input_audio_mediaplayer.cpp
              lib/fftreal/fftreal.cpp
              lib/RtAudio/RtAudio.cpp)

  add_library(${module_id} MODULE ${SOURCES})
  include(../cmake_suffix.txt)
  target_link_libraries(${module_id} ${AUDIO_LIBRARIES} ${PTHREAD_LIBRARY})
endif(AUDIO_LIBRARIES)

