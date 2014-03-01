cmake_minimum_required(VERSION 2.6)

if(WIN32)
  # compute module id from compilation path
  get_filename_component(list_file_path ${CMAKE_CURRENT_LIST_FILE} PATH)
  string(REGEX MATCH "[a-z0-9.-]*$" module_id ${list_file_path})

  project (${module_id})

  message("configuring vsxu module            " ${module_id})

  # set defines for MOD_CM, MOD_DM, MOD_NM
  CREATE_STATIC_GLUE(${module_id})

  # find the local source files
  file(GLOB LOCAL_SOURCES "*.cpp")

  # include common
  include(../cmake_globals.txt)

  # actual compilation target
  CREATE_MODULE(${module_id})

  # include common
  include(../cmake_suffix.txt)

  CREATE_TARGET_LINK_LIBRARIES(
    gdi32
    avifil32
  )

endif(WIN32)
