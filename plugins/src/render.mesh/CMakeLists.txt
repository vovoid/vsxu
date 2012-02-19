cmake_minimum_required(VERSION 2.6)
#message("current list file is: " ${CMAKE_CURRENT_LIST_FILE})

get_filename_component(list_file_path ${CMAKE_CURRENT_LIST_FILE} PATH)
string(REGEX MATCH "[a-z.-]*$" module_id ${list_file_path})

message("configuring vsxu module            " ${module_id})


CREATE_STATIC_GLUE(${module_id})

file(GLOB LOCAL_SOURCES "*.cpp")
file(GLOB API_STUBS "api_stubs/*.cpp")

project (${module_id})
include(./_sources)
include (../cmake_globals.txt)
CREATE_MODULE(${module_id})

include(../cmake_suffix.txt)

#target_link_libraries(${module_id} vsxu_engine_graphics vsxu_engine)
