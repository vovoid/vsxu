if(WIN32)
cmake_minimum_required(VERSION 2.6)

get_filename_component(list_file_path ${CMAKE_CURRENT_LIST_FILE} PATH)
string(REGEX MATCH "[a-z.-]*$" module_id ${list_file_path})
message("*******************************************")
message("configuring vsxu module            " ${module_id})
message("*******************************************")


project (${module_id})

include (../cmake_globals.txt)
include(${list_file_path}/_sources)
include_directories(${CMAKE_SOURCE_DIR}/engine_graphics/include)

add_library(${module_id} MODULE ${SOURCES})
include(../cmake_suffix.txt)

target_link_libraries(${module_id}
gdi32
avifil32
)
endif(WIN32)
