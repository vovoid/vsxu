cmake_minimum_required(VERSION 2.6)

get_filename_component(list_file_path ${CMAKE_CURRENT_LIST_FILE} PATH)
string(REGEX MATCH "[a-z.-]*$" module_id ${list_file_path})

message("configuring vsxu module            " ${module_id})


file(GLOB LOCAL_SOURCES "*.cpp")
file(GLOB OCEAN "ocean/*.cpp")
file(GLOB METABALLS "metaballs/*.cpp")

project (${module_id})
include (../cmake_globals.txt)
add_library(${module_id} MODULE ${LOCAL_SOURCES} ${OCEAN} ${METABALLS} ${SOURCES})
include(../cmake_suffix.txt)
target_link_libraries(${module_id} vsxu_engine)
