cmake_minimum_required(VERSION 2.6)
#message("current list file is: " ${CMAKE_CURRENT_LIST_FILE})

get_filename_component(list_file_path ${CMAKE_CURRENT_LIST_FILE} PATH)
string(REGEX MATCH "[a-z.-]*$" module_id ${list_file_path})

message("configuring vsxu module            " ${module_id})


set(LOCAL_SOURCES "main_mesh.modifiers.cpp")
include (../cmake_globals.txt)
project (${module_id})
add_library(${module_id} MODULE ${LOCAL_SOURCES} ${API_STUBS})
include(../cmake_suffix.txt)
