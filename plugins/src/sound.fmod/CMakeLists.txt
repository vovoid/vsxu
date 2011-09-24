cmake_minimum_required(VERSION 2.6)
#message("current list file is: " ${CMAKE_CURRENT_LIST_FILE})
include(../cmake_globals.txt)

get_filename_component(list_file_path ${CMAKE_CURRENT_LIST_FILE} PATH)
string(REGEX MATCH "[a-z.-]*$" module_id ${list_file_path})
message("*******************************************")
message("configuring vsxu module            " ${module_id})
message("*******************************************")

if(UNIX)
  include_directories(lib/linux32/api/inc/)
endif(UNIX)

if(WIN32)
  include_directories(lib/win32/api/inc/)
  link_directories(lib/win32/api/lib/)
endif(WIN32)

file(GLOB LOCAL_SOURCES "*.cpp")
file(GLOB API_STUBS "api_stubs/*.cpp")

link_directories(
../../
../../vsxfst
)


project (${module_id})
include(_sources)
add_library(${module_id} MODULE ${LOCAL_SOURCES} ${API_STUBS} ${SOURCES} ${EXTRA_LIBS} "fftreal/fftreal.cpp")
include(../cmake_suffix.txt)
target_link_libraries(${module_id} fmodex vsxu_engine)