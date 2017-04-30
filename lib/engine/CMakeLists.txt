set(module_id vsx_engine)

message("configuring            " ${module_id})


################################################################################
project (${module_id})

################################################################################
# CMAKE PACKAGES ###############################################################
################################################################################
find_package(DL REQUIRED)

################################################################################
# INCLUDES #####################################################################
################################################################################

include_directories(
  ${CMAKE_SOURCE_DIR}/
  ${CMAKE_SOURCE_DIR}/lib/common/include
  ${CMAKE_SOURCE_DIR}/lib/engine/include
  ${CMAKE_SOURCE_DIR}/lib/engine_graphics/include
  include
  src/log
  src/vsx_comp
  src
)

file(GLOB_RECURSE HEADER_FILES *.h)

################################################################################
# DEFINES
################################################################################
# - if you want an engine without OpenGL linkage, add -DVSX_NO_GL
################################################################################
# To debug the string library, enable this:
# add_definitions(
#  -DVSXS_DEBUG=1
# )

if (NOT MSVC)
  add_definitions( -Wno-parentheses ) # LZMA
  #add_definitions( -Wno-int-to-pointer-cast ) # vsx_filesystem::filesystemst WIN64
  add_definitions( -Wno-error -fpermissive ) # vsx_filesystem::filesystemst WIN64
endif()

add_definitions(
  # -DBUILDING_DLL
 -DVSXU_MODULE_TIMING
 -DVSXU_ARTISTE
 -DVSXU_DEVELOPER
 -DCMAKE_INSTALL_PREFIX="${CMAKE_INSTALL_PREFIX}"
 -DMOD_CNM=create_new_module
 -DMOD_DM=destroy_module
 -DMOD_GNM=get_num_modules
) 


if (VSXU_DEBUG EQUAL 1)
  add_definitions( -DVSXU_DEBUG )
endif (VSXU_DEBUG EQUAL 1)

################################################################################
# SOURCES ######################################################################
################################################################################

if(ENGINE_SCRIPTING)
# add gamemonkey source files here
  include_directories(
    src/core/scripting/game_monkey
    src/core/scripting/game_monkey/gm
  )
  set(GAME_MONKEY 
    src/core/scripting/game_monkey/binds/gmStringLib.cpp
    src/core/scripting/game_monkey/binds/gmGCRootUtil.cpp
    src/core/scripting/game_monkey/binds/gmHelpers.cpp
    src/core/scripting/game_monkey/binds/gmVector3Lib.cpp
    src/core/scripting/game_monkey/binds/gmGCRoot.cpp
    src/core/scripting/game_monkey/binds/gmArrayLib.cpp
    src/core/scripting/game_monkey/binds/gmCall.cpp
    src/core/scripting/game_monkey/binds/gmMathLib.cpp
    src/core/scripting/game_monkey/gm/gmMachineLib.cpp
    src/core/scripting/game_monkey/gm/gmStreamBuffer.cpp
    src/core/scripting/game_monkey/gm/gmParser.cpp
    src/core/scripting/game_monkey/gm/gmHash.cpp
    src/core/scripting/game_monkey/gm/gmCodeGen.cpp
    src/core/scripting/game_monkey/gm/gmArraySimple.cpp
    src/core/scripting/game_monkey/gm/gmThread.cpp
    src/core/scripting/game_monkey/gm/gmLog.cpp
    src/core/scripting/game_monkey/gm/gmMemFixedSet.cpp
    #src/core/scripting/game_monkey/gm/gmDebugger.cpp
    src/core/scripting/game_monkey/gm/gmStringObject.cpp
    src/core/scripting/game_monkey/gm/gmCodeTree.cpp
    src/core/scripting/game_monkey/gm/gmStream.cpp
    src/core/scripting/game_monkey/gm/gmVariable.cpp
    src/core/scripting/game_monkey/gm/gmParser.cpp.h
    src/core/scripting/game_monkey/gm/gmCodeGenHooks.cpp
    src/core/scripting/game_monkey/gm/gmDebug.cpp
    src/core/scripting/game_monkey/gm/gmIncGC.cpp
    src/core/scripting/game_monkey/gm/gmLibHooks.cpp
    src/core/scripting/game_monkey/gm/gmMem.cpp
    src/core/scripting/game_monkey/gm/gmTableObject.cpp
    src/core/scripting/game_monkey/gm/gmMachine.cpp
    src/core/scripting/game_monkey/gm/gmMemChain.cpp
    src/core/scripting/game_monkey/gm/gmUtil.cpp
    src/core/scripting/game_monkey/gm/gmFunctionObject.cpp
    src/core/scripting/game_monkey/gm/gmCrc.cpp
    src/core/scripting/game_monkey/gm/gmByteCode.cpp
    src/core/scripting/game_monkey/gm/gmListDouble.cpp
    src/core/scripting/game_monkey/gm/gmOperators.cpp
    src/core/scripting/game_monkey/gm/gmMemFixed.cpp
    src/core/scripting/game_monkey/gm/gmByteCodeGen.cpp
    src/core/scripting/game_monkey/gm/gmUserObject.cpp
    src/core/scripting/game_monkey/gm/gmScanner.cpp
    src/core/scripting/vsx_comp_vsxl.cpp
    src/core/scripting/vsx_param_vsxl.cpp
  )
else(ENGINE_SCRIPTING)
  set(GAME_MONKEY "")
  add_definitions(
    -DVSXE_NO_GM
  )
endif(ENGINE_SCRIPTING)

set(SOURCES
  src/vsx_engine.cpp
  src/vsx_engine_abs.cpp
  src/vsx_sequence_pool.cpp
  src/vsx_param_abstraction.cpp
  src/vsx_comp_channel.cpp
  src/vsx_param_interpolation.cpp
  src/vsx_comp/vsx_comp.cpp
  src/vsx_param_sequence.cpp
  src/vsx_master_sequencer/vsx_master_sequence_channel.cpp
  src/vsx_param_sequence_list.cpp
  src/vsx_module_list/vsx_module_list_factory.cpp
  src/vsx_module_list/vsx_dlopen.cpp
  src/vsx_param.cpp
)

################################################################################
# LINKAGE ######################################################################
################################################################################

if (VSXU_STATIC EQUAL 1)

  #static build
  add_library(${module_id} STATIC ${SOURCES} ${HEADER_FILES} ${GAME_MONKEY})
  target_link_libraries(${module_id} plugins)

else ()

  #dynamic build
  add_library(${module_id} SHARED ${SOURCES} ${HEADER_FILES} ${GAME_MONKEY})
endif ()

if (UNIX)
  install(TARGETS ${module_id} DESTINATION ${VSXU_INSTALL_LIB_DIR} COMPONENT engine)

######################################################################################
# Install the Headers so as to install VSXu as a library
  install (DIRECTORY "include/" DESTINATION ${CMAKE_INSTALL_PREFIX}/${VSXU_INSTALL_INCLUDE_DIR}/engine COMPONENT Devel)
################################################################################
endif()

target_link_libraries(${module_id}
  ${DL_LIBRARY}
  vsx_common
)

