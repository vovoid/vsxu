if(WIN32)
cmake_minimum_required(VERSION 2.6)
#message("current list file is: " ${CMAKE_CURRENT_LIST_FILE})

get_filename_component(list_file_path ${CMAKE_CURRENT_LIST_FILE} PATH)
string(REGEX MATCH "[a-z.-]*$" module_id ${list_file_path})
message("*******************************************")
message("building vsxu module            " ${module_id})
message("*******************************************")


project (${module_id})

include(./_sources)
add_library(${module_id} MODULE ${SOURCES})
include(../cmake_suffix.txt)

target_link_libraries(${module_id}
gdi32
vfw32
)
endif(WIN32)
