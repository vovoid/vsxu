cmake_minimum_required(VERSION 2.6)

get_filename_component(list_file_path ${CMAKE_CURRENT_LIST_FILE} PATH)

string(REGEX MATCH "[a-z.-]*$" module_id ${list_file_path})

message("configuring vsxu module            " ${module_id})


CREATE_STATIC_GLUE(${module_id})

file(GLOB LOCAL_SOURCES "*.cpp")

project (${module_id})

include(../cmake_globals.txt)
include(./_sources)

CREATE_MODULE(${module_id})

include(../cmake_suffix.txt)
