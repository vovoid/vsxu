cmake_minimum_required(VERSION 2.8)
find_package( OpenCV )
find_package( Threads )

if(OpenCV_FOUND)

cmake_minimum_required(VERSION 2.6)

# compute module id from compilation path
get_filename_component(list_file_path ${CMAKE_CURRENT_LIST_FILE} PATH)
string(REGEX MATCH "[a-z0-9._-]*$" module_id ${list_file_path})

project (${module_id})

message("configuring vsxu module            " ${module_id})

# set defines for MOD_CM, MOD_DM, MOD_NM
CREATE_STATIC_GLUE(${module_id})

include_directories(${OpenCV_INCLUDE_DIRS})

CREATE_TARGET_LINK_DIRECTORIES(${OpenCV_LIB_DIR})

# find the local source files
set(SOURCES main.cpp
            input/input_video.cpp
            input/input_video_file.cpp
            input/input_video_camera.cpp
            trackers/tracker_bitmap_color.cpp)

# include common
include(../cmake_globals.txt)


# actual compilation target
CREATE_MODULE(${module_id})


set(LIBRARIES
  ${OpenCV_LIBS} ${CMAKE_THREAD_LIBS_INIT}
)

# include common
include(../cmake_suffix.txt)

endif(OpenCV_FOUND)
