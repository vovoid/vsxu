cmake_minimum_required(VERSION 2.6)
#message("current list file is: " ${CMAKE_CURRENT_LIST_FILE})

get_filename_component(list_file_path ${CMAKE_CURRENT_LIST_FILE} PATH)
string(REGEX MATCH "[a-z._-]*$" module_id ${list_file_path})

message("configuring vsxu module            " ${module_id})


file(GLOB LOCAL_SOURCES "*.cpp")
file(GLOB API_STUBS "api_stubs/*.cpp")

project (${module_id})
include (../cmake_globals.txt)
include(./_sources)
add_library(${module_id} MODULE ${LOCAL_SOURCES} fftreal/fftreal.cpp ${API_STUBS} ${SOURCES})
include(../cmake_suffix.txt)
