cmake_minimum_required(VERSION 2.8)

get_filename_component(list_file_path ${CMAKE_CURRENT_LIST_FILE} PATH)
string(REGEX MATCH "[a-z.-]*$" module_id ${list_file_path})

message("configuring vsxu module            " ${module_id})

project (${module_id})

include (../cmake_globals.txt)

set(SOURCES main.cpp module_video_file.cpp module_video_camera.cpp module_video_input.cpp)

include_directories(${OpenCV_INCLUDE_DIRS})

link_directories(${OpenCV_LIB_DIR})
add_library(${module_id} MODULE ${SOURCES})
include(../cmake_suffix.txt)
target_link_libraries(${module_id} ${OpenCV_LIBS})

