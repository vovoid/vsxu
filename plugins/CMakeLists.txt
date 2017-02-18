cmake_minimum_required(VERSION 2.8)

function(CREATE_STATIC_GLUE module_id)
  if (VSXU_STATIC EQUAL 1)
    # create a link directory
    file(APPEND ../../static_factory_cmake_link_directories "link_directories(" ${CMAKE_SOURCE_DIR}/plugins/src/${module_id}/ ")\n" )

    # link library
    file(APPEND ../../static_factory_cmake_target_link_libraries "target_link_libraries(\${module_id} " ${module_id} ")\n" )

    # pkgconfig lib
    file(APPEND ../../static_factory_pkgconfig_libs "-l:${module_id}.a ")

    # make a C++-friendly name out of the module name
    string(REPLACE "." "_" module_id_f ${module_id} )

    # rig up the factory include symbols
    # create defines for code in the engine
    file(APPEND ../../static_factory_funcmapping.h
      "vsx_module* " vsx_module_${module_id_f}_cm "(unsigned long module, void* args);\n"
      "void " vsx_module_${module_id_f}_dm "(vsx_module* m, unsigned long module);\n"
      "unsigned long " vsx_module_${module_id_f}_nm "(vsx_module_engine_environment* environment);\n"
    )
    # create a cpp include file for calls
    file(APPEND ../../static_factory_funcmapping.cpp "  VSXM_SF_ADD_MODULE(\"${module_id}\"," vsx_module_${module_id_f}_cm "," vsx_module_${module_id_f}_dm "," vsx_module_${module_id_f}_nm ");\n")
    add_definitions(
      -DMOD_CM=vsx_module_${module_id_f}_cm
      -DMOD_DM=vsx_module_${module_id_f}_dm
      -DMOD_NM=vsx_module_${module_id_f}_nm
    )


  else  ()


    add_definitions(
      -DMOD_CM=create_new_module
      -DMOD_DM=destroy_module
      -DMOD_NM=get_num_modules
    )
  endif ()

endfunction(CREATE_STATIC_GLUE)


function( CREATE_MODULE ${module_id} )
  if (VSXU_STATIC EQUAL 1)

      GET_PROPERTY(LOCAL_GLOBAL_SOURCES GLOBAL PROPERTY GLOBAL_PLUGIN_SOURCES)
      SET_PROPERTY(GLOBAL PROPERTY GLOBAL_PLUGIN_SOURCES ${LOCAL_GLOBAL_SOURCES} ${LOCAL_SOURCES} ${API_STUBS} ${SOURCES} ${HEADER_FILES})

      GET_PROPERTY(LOCAL_GLOBAL_DEFINITIONS GLOBAL PROPERTY GLOBAL_PLUGIN_DEFINITIONS)
      SET_PROPERTY(GLOBAL PROPERTY GLOBAL_PLUGIN_DEFINITIONS ${LOCAL_GLOBAL_DEFINITIONS} ${DEFINITIONS})

      GET_PROPERTY(LOCAL_GLOBAL_INCLUDE_DIRS GLOBAL PROPERTY GLOBAL_PLUGIN_INCLUDE_DIRS)
      SET_PROPERTY(GLOBAL PROPERTY GLOBAL_PLUGIN_INCLUDE_DIRS ${LOCAL_GLOBAL_INCLUDE_DIRS} ${INCLUDE_DIRS})

  else (VSXU_STATIC EQUAL 1)
    add_library(${module_id} MODULE ${LOCAL_SOURCES} ${API_STUBS} ${SOURCES} ${HEADER_FILES})
  endif (VSXU_STATIC EQUAL 1)
endfunction(CREATE_MODULE)


if (VSXU_STATIC EQUAL 1)
  # 1. create the files for static factory
  file(WRITE static_factory_funcmapping.cpp "")
  file(WRITE static_factory_funcmapping.h "")
  file(WRITE static_factory_cmake_link_directories "")
  file(WRITE static_factory_cmake_target_link_libraries "")
  file(WRITE static_factory_pkgconfig_libs "")
endif()

if (WIN32 AND VSXU_STATIC EQUAL 1)
  add_definitions(-DGLEW_STATIC=1)
  add_definitions(-DFTGL_LIBRARY_STATIC -DFT2_BUILD_LIBRARY)
endif()

add_subdirectory(src/bitmap.generators)
add_subdirectory(src/bitmap.loaders)
add_subdirectory(src/bitmap.modifiers)
add_subdirectory(src/_deprecated)
add_subdirectory(src/math)
add_subdirectory(src/math.oscillators)
add_subdirectory(src/mesh.generators)
add_subdirectory(src/mesh.importers)
add_subdirectory(src/mesh.modifiers)
add_subdirectory(src/mesh.render)
add_subdirectory(src/outputs.screen_opengl)
add_subdirectory(src/particlesystem.generators)
add_subdirectory(src/particlesystem.modifiers)
add_subdirectory(src/particlesystem.render)
add_subdirectory(src/render.basic)
add_subdirectory(src/render.glsl)
add_subdirectory(src/render.gravity_lines)
add_subdirectory(src/render.opengl)
add_subdirectory(src/render.text)
add_subdirectory(src/selectors)

if (VSXU_STATIC_AUDIO_SINK)
  add_definitions(-DVSXU_AUDIO_SINK=1)
  add_subdirectory(src/sound.sink)
else()
  add_subdirectory(src/sound.rtaudio)
endif()

add_subdirectory(src/string)
add_subdirectory(src/system)
add_subdirectory(src/texture)

if (VSXU_STATIC EQUAL 1)
  GET_PROPERTY(SOURCES GLOBAL PROPERTY GLOBAL_PLUGIN_SOURCES)
  GET_PROPERTY(DEFINITIONS GLOBAL PROPERTY GLOBAL_PLUGIN_DEFINITIONS)
  GET_PROPERTY(INCLUDE_DIRS GLOBAL PROPERTY GLOBAL_PLUGIN_INCLUDE_DIRS)
  include_directories(${INCLUDE_DIRS})
  add_definitions(${DEFINITIONS})
  include(${CMAKE_CURRENT_LIST_DIR}/src/cmake_globals.txt)
#  message(${SOURCES})
  add_library(plugins STATIC ${SOURCES})
endif()


