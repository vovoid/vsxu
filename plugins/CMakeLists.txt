cmake_minimum_required(VERSION 2.8)

function(CREATE_STATIC_GLUE module_id)
  if (VSXU_ENGINE_STATIC EQUAL 1)
    # create a link directory
    file(APPEND ../../static_factory_cmake_link_directories "link_directories(" ${CMAKE_SOURCE_DIR}/plugins/src/${module_id}/")\n" )
    # link library
    file(APPEND ../../static_factory_cmake_target_link_libraries "target_link_libraries(\${module_id} " ${module_id} ")\n" )

    # make a C++-friendly name out of the module name
    string(REPLACE "." "_" module_id_f ${module_id} )

    # rig up the factory include symbols
    # create defines for code in the engine
    file(APPEND ../../static_factory_funcmapping.h
      "vsx_module* " vsx_module_${module_id_f}_cm "(unsigned long module, void* args);\n"
      "void " vsx_module_${module_id_f}_dm "(vsx_module* m, unsigned long module);\n"
      "unsigned long " vsx_module_${module_id_f}_nm "();\n"
    )
    # create a cpp include file for calls
    file(APPEND ../../static_factory_funcmapping.cpp "  VSXM_SF_ADD_MODULE(\"${module_id}\"," vsx_module_${module_id_f}_cm "," vsx_module_${module_id_f}_dm "," vsx_module_${module_id_f}_nm ");\n")
    add_definitions(
      -DMOD_CM=vsx_module_${module_id_f}_cm
      -DMOD_DM=vsx_module_${module_id_f}_dm
      -DMOD_NM=vsx_module_${module_id_f}_nm
    )
  else  (VSXU_ENGINE_STATIC EQUAL 1)
    add_definitions(
      -DMOD_CM=create_new_module
      -DMOD_DM=destroy_module
      -DMOD_NM=get_num_modules
    )
  endif (VSXU_ENGINE_STATIC EQUAL 1)
endfunction(CREATE_STATIC_GLUE)

if (VSXU_TM)
  find_package(Tm REQUIRED)
  add_definitions(-DVSXU_TM)

  include_directories(
    ${TM_INCLUDE_PATH}
  )
endif()


function( CREATE_MODULE ${module_id} )
  if (VSXU_ENGINE_STATIC EQUAL 1)
    add_library(${module_id} STATIC ${LOCAL_SOURCES} ${API_STUBS} ${SOURCES})
  else (VSXU_ENGINE_STATIC EQUAL 1)
    add_library(${module_id} MODULE ${LOCAL_SOURCES} ${API_STUBS} ${SOURCES})
  endif (VSXU_ENGINE_STATIC EQUAL 1)
endfunction(CREATE_MODULE)

function( CREATE_TARGET_LINK_DIRECTORIES la)
  if (VSXU_ENGINE_STATIC EQUAL 1)
    if (NOT ${ARGV0} STREQUAL "")
      file(APPEND ../../static_factory_cmake_target_link_libraries "link_directories(" ${ARGV0} ")\n" )
    endif()

    if (NOT ${ARGV1} STREQUAL "")
      file(APPEND ../../static_factory_cmake_target_link_libraries "link_directories(" ${ARGV1} ")\n" )
    endif()

    if (NOT ${ARGV2} STREQUAL "")
      file(APPEND ../../static_factory_cmake_target_link_libraries "link_directories(" ${ARGV2} ")\n" )
    endif()

    if (NOT ${ARGV3} STREQUAL "")
      file(APPEND ../../static_factory_cmake_target_link_libraries "link_directories(" ${ARGV3} ")\n" )
    endif()

    if (NOT ${ARGV4} STREQUAL "")
      file(APPEND ../../static_factory_cmake_target_link_libraries "link_directories(" ${ARGV4} ")\n" )
    endif()

    if (NOT ${ARGV5} STREQUAL "")
      file(APPEND ../../static_factory_cmake_target_link_libraries "link_directories(" ${ARGV5} ")\n" )
    endif()

    if (NOT ${ARGV6} STREQUAL "")
      file(APPEND ../../static_factory_cmake_target_link_libraries "link_directories(" ${ARGV6} ")\n" )
    endif()

  endif()
  link_directories(${module_id} ${ARGV0} ${ARGV1} ${ARGV2} ${ARGV3} ${ARGV4} ${ARGV5} ${ARGV6})

endfunction()


function( CREATE_TARGET_LINK_LIBRARIES la)
  if (VSXU_ENGINE_STATIC EQUAL 1)
    if (NOT ${ARGV0} STREQUAL "")
      file(APPEND ../../static_factory_cmake_target_link_libraries "target_link_libraries(\${module_id} " ${ARGV0} ")\n" )
    endif()

    if (NOT ${ARGV1} STREQUAL "")
      file(APPEND ../../static_factory_cmake_target_link_libraries "target_link_libraries(\${module_id} " ${ARGV1} ")\n" )
    endif()

    if (NOT ${ARGV2} STREQUAL "")
      file(APPEND ../../static_factory_cmake_target_link_libraries "target_link_libraries(\${module_id} " ${ARGV2} ")\n" )
    endif()

    if (NOT ${ARGV3} STREQUAL "")
      file(APPEND ../../static_factory_cmake_target_link_libraries "target_link_libraries(\${module_id} " ${ARGV3} ")\n" )
    endif()

    if (NOT ${ARGV4} STREQUAL "")
      file(APPEND ../../static_factory_cmake_target_link_libraries "target_link_libraries(\${module_id} " ${ARGV4} ")\n" )
    endif()

    if (NOT ${ARGV5} STREQUAL "")
      file(APPEND ../../static_factory_cmake_target_link_libraries "target_link_libraries(\${module_id} " ${ARGV5} ")\n" )
    endif()

    if (NOT ${ARGV6} STREQUAL "")
      file(APPEND ../../static_factory_cmake_target_link_libraries "target_link_libraries(\${module_id} " ${ARGV6} ")\n" )
    endif()

    if (NOT ${ARGV7} STREQUAL "")
      file(APPEND ../../static_factory_cmake_target_link_libraries "target_link_libraries(\${module_id} " ${ARGV7} ")\n" )
    endif()

    if (NOT ${ARGV8} STREQUAL "")
      file(APPEND ../../static_factory_cmake_target_link_libraries "target_link_libraries(\${module_id} " ${ARGV8} ")\n" )
    endif()

    if (NOT ${ARGV9} STREQUAL "")
      file(APPEND ../../static_factory_cmake_target_link_libraries "target_link_libraries(\${module_id} " ${ARGV9} ")\n" )
    endif()

    if (NOT ${ARGV10} STREQUAL "")
      file(APPEND ../../static_factory_cmake_target_link_libraries "target_link_libraries(\${module_id} " ${ARGV10} ")\n" )
    endif()

    if (NOT ${ARGV11} STREQUAL "")
      file(APPEND ../../static_factory_cmake_target_link_libraries "target_link_libraries(\${module_id} " ${ARGV11} ")\n" )
    endif()

    if (NOT ${ARGV12} STREQUAL "")
      file(APPEND ../../static_factory_cmake_target_link_libraries "target_link_libraries(\${module_id} " ${ARGV12} ")\n" )
    endif()

    if (NOT ${ARGV13} STREQUAL "")
      file(APPEND ../../static_factory_cmake_target_link_libraries "target_link_libraries(\${module_id} " ${ARGV13} ")\n" )
    endif()

    if (NOT ${ARGV14} STREQUAL "")
      file(APPEND ../../static_factory_cmake_target_link_libraries "target_link_libraries(\${module_id} " ${ARGV14} ")\n" )
    endif()

    return()
  endif()

  target_link_libraries(${module_id} ${ARGV0} ${ARGV1} ${ARGV2} ${ARGV3} ${ARGV4} ${ARGV5} ${ARGV6})


endfunction()


if (VSXU_OPENGL_ES_2_0 EQUAL 1)
  add_definitions(-DVSXU_OPENGL_ES)
  add_definitions(-DVSXU_OPENGL_ES_2_0)
endif (VSXU_OPENGL_ES_2_0 EQUAL 1)


#GROWL
if (VSXU_ENGINE_STATIC EQUAL 1)
  # 1. create the file for static factory
  file(WRITE static_factory_funcmapping.cpp "")
  file(WRITE static_factory_funcmapping.h "")
  file(WRITE static_factory_cmake_link_directories "")
  file(WRITE static_factory_cmake_target_link_libraries "")
endif()

add_subdirectory(src/bitmap.import.avi)
add_subdirectory(src/bitmap.modifiers)
add_subdirectory(src/bitmap.generators)
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
add_subdirectory(src/render.skybox)
add_subdirectory(src/render.text)
add_subdirectory(src/sound.rtaudio)
add_subdirectory(src/system)
add_subdirectory(src/texture)
add_subdirectory(src/vision)

if (PLUGINS_EXPERIMENTAL_SENSORS)
  add_subdirectory(src/sensors.accelerometer)
endif()


