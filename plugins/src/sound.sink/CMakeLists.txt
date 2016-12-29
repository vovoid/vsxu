cmake_minimum_required(VERSION 2.6)

get_filename_component(list_file_path ${CMAKE_CURRENT_LIST_FILE} PATH)
string(REGEX MATCH "[a-z0-9._-]*$" module_id ${list_file_path})

message("configuring vsxu module            " ${module_id})

project (${module_id})
include (../cmake_globals.txt)

# set defines for MOD_CM, MOD_DM, MOD_NM
CREATE_STATIC_GLUE(${module_id})

file(GLOB LOCAL_SOURCES "*.cpp")
file(GLOB_RECURSE HEADER_FILES *.h)

# actual compilation target
CREATE_MODULE(${module_id})

include(../cmake_suffix.txt)

if (not MSVC)
add_definitions(
        -Wno-parentheses
        -Wno-unused-value
        -Wno-unused-variable
        -Wno-unused-function
        -Wno-unused-but-set-variable
        )
endif()
