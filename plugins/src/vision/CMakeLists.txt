cmake_minimum_required(VERSION 2.8)
find_package( OpenCV )
find_package( Threads )

if(OpenCV_FOUND)

get_filename_component(list_file_path ${CMAKE_CURRENT_LIST_FILE} PATH)
string(REGEX MATCH "[a-z.-]*$" module_id ${list_file_path})

message("configuring vsxu module            " ${module_id})

project (${module_id})

include (../cmake_globals.txt)

set(SOURCES main.cpp
            input/input_video.cpp
            input/input_video_file.cpp
            input/input_video_camera.cpp
            trackers/tracker_bitmap_color.cpp)

include_directories(${OpenCV_INCLUDE_DIRS})

link_directories(${OpenCV_LIB_DIR})
add_library(${module_id} MODULE ${SOURCES})
target_link_libraries(${module_id} ${OpenCV_LIBS} ${CMAKE_THREAD_LIBS_INIT})
include(../cmake_suffix.txt)

endif(OpenCV_FOUND)
