cmake_minimum_required(VERSION 2.6)

include(../cmake_globals.txt)

include_directories(
  ../
  ../engine/include
  ../engine_graphics/include
  src
)

set(OS_SOURCES "")

if(UNIX)
  if (VSXU_OPENGL_ES_2_0 EQUAL 1)
    if (MEEGO EQUAL 1)
      include_directories(src/main/linux-opengl/)
      add_definitions(-DVSXU_OPENGL_ES)
      set(OS_SOURCES
        src/main/linux-opengl_es/main.cpp
      )
    else (VSXU_MEEGO EQUAL 1)
      include_directories(src/main/linux-opengl/)
      add_definitions(-DVSXU_OPENGL_ES)
      set(OS_SOURCES
        src/main/linux-meego_opengl_es/main.cpp
      )
    endif (MEEGO EQUAL 1)
  else (VSXU_OPENGL_ES_2_0 EQUAL 1)
    set(OS_SOURCES
      src/main/linux-opengl/main.cpp
    )
  endif (VSXU_OPENGL_ES_2_0 EQUAL 1)

endif(UNIX)


if(VSXU_DEBUG)
add_definitions(
 -DDEBUG
)
endif(VSXU_DEBUG)

if (PLATFORM_SHARED_FILES_FLAT_INSTALL EQUAL 1)
  add_definitions( -DPLATFORM_SHARED_FILES_FLAT_INSTALL )
endif (PLATFORM_SHARED_FILES_FLAT_INSTALL EQUAL 1)

add_definitions(
 -DVSXU_EXE
 -DVSXU_ARTISTE
 -DVSX_FONT_NO_FT
 -DCMAKE_INSTALL_PREFIX="${CMAKE_INSTALL_PREFIX}"
)

get_filename_component(list_file_path ${CMAKE_CURRENT_LIST_FILE} PATH)
string(REGEX MATCH "[a-z._-]*$" module_id ${list_file_path})
message("*******************************************")
message("configuring            " ${module_id})
message("*******************************************")

set(SOURCES
  src/application.cpp
)

link_directories(
../
../engine
)

if (VSXU_ENGINE_STATIC EQUAL 1)
  include(../plugins/static_factory_cmake_link_directories)
  link_directories(../lib/linux32/)
endif (VSXU_ENGINE_STATIC EQUAL 1)


project (${module_id})

add_executable(${module_id}  ${SOURCES} ${OS_SOURCES} ${EXTRA_LIBS})
include(../cmake_suffix.txt)

set_target_properties(
  ${module_id}
    PROPERTIES
      OUTPUT_NAME
        vsxu_server
)

if(UNIX)

  
  link_directories("/usr/lib/vsxu")

  if (VSXU_OPENGL_ES_2_0 EQUAL 1)
    target_link_libraries(${module_id}
      EGL
      GLESv2
      X11
      png
      jpeg
    )
  else (VSXU_OPENGL_ES_2_0 EQUAL 1)
    target_link_libraries(${module_id}
      Xrandr
      X11
      vovoid_glfw
      pthread
      GLU
      GL
      GLEW
      png
      jpeg
    )
  endif (VSXU_OPENGL_ES_2_0 EQUAL 1)

  if (VSXU_ENGINE_STATIC EQUAL 1)
    message("ENGINE STATIC")
    target_link_libraries(${module_id} vsxu_engine)
    target_link_libraries(${module_id} vsxu_engine_graphics)
    include(../plugins/static_factory_cmake_target_link_libraries)
  else (VSXU_ENGINE_STATIC EQUAL 1)
    include(../plugins/static_factory_cmake_target_link_libraries)
    target_link_libraries(${module_id} vsxu_engine)
    target_link_libraries(${module_id} vsxu_engine_graphics)
  endif (VSXU_ENGINE_STATIC EQUAL 1)

  install(TARGETS ${module_id} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
endif(UNIX)
