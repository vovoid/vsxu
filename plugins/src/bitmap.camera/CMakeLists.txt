cmake_minimum_required(VERSION 2.6)
find_package (OpenCV)
if(OpenCV_FOUND)

  get_filename_component(list_file_path ${CMAKE_CURRENT_LIST_FILE} PATH)
  string(REGEX MATCH "[a-z.-]*$" module_id ${list_file_path})
  message("*******************************************")
  message("configuring vsxu module            " ${module_id})
  message("*******************************************")


  project (${module_id})

  include (../cmake_globals.txt)
  include(${list_file_path}/_sources)
  include_directories(${OpenCV_INCLUDE_DIRS})

  link_directories(${OpenCV_LIB_DIR})
  add_library(${module_id} MODULE ${SOURCES})
  include(../cmake_suffix.txt)
  target_link_libraries(${module_id} ${OpenCV_LIBS})

endif(OpenCV_FOUND)
