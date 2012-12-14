cmake_minimum_required(VERSION 2.6)

get_filename_component(list_file_path ${CMAKE_CURRENT_LIST_FILE} PATH)
string(REGEX MATCH "[a-z._-]*$" module_id ${list_file_path})
message("configuring vsxu module            " ${module_id})


set(AUDIO_LIBRARIES "")

find_package(PULSEAUDIO)
if (PULSEAUDIO_FOUND)
  add_definitions(-D__LINUX_PULSE__)
  include_directories(${PULSEAUDIO_INCLUDE_DIRS})
  set( AUDIO_LIBRARIES ${PULSEAUDIO_LIBRARY} ${PULSEAUDIO_SIMPLE_LIBRARY})
endif (PULSEAUDIO_FOUND)

#TODO: Add in the needed definitions for each Audio Library on each platform
if(AUDIO_LIBRARIES)
  project (${module_id})
  find_package(PTHREAD)
  include (../cmake_globals.txt)
  include_directories ( lib/RtAudio/
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

