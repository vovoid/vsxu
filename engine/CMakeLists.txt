set(module_id vsxu_engine)

message("configuring            " ${module_id})


################################################################################
project (${module_id})

################################################################################
# CMAKE PACKAGES ###############################################################
################################################################################
find_package(DL REQUIRED)
if (WIN32)
  find_package(PTHREAD REQUIRED)
else()
  find_package(Threads REQUIRED)
endif()

################################################################################
# INCLUDES #####################################################################
################################################################################

include_directories(
  ${GLEW_INCLUDE_PATH}
  ../
  ../engine_graphics/include
  ../profiler/include
  include
  src/log
  src/core/vsx_comp
  src/core
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

add_definitions(
 -DBUILDING_DLL
 -DVSXU_MODULE_TIMING
 -DVSXU_ARTISTE
 -DVSXU_DEVELOPER
 -DCMAKE_INSTALL_PREFIX="${CMAKE_INSTALL_PREFIX}"
 -DMOD_CNM=create_new_module
 -DMOD_DM=destroy_module
 -DMOD_GNM=get_num_modules
) 

if (VSXU_OPENGL_ES_2_0 EQUAL 1)
  add_definitions(-DVSXU_OPENGL_ES -DVSXU_OPENGL_ES_2_0)
endif (VSXU_OPENGL_ES_2_0 EQUAL 1)


if (VSXU_DEBUG EQUAL 1)
  add_definitions( -DVSXU_DEBUG )
endif (VSXU_DEBUG EQUAL 1)


if(CMAKE_SYSTEM_NAME MATCHES "Linux")
  add_definitions(-DVSXU_PLUGIN_LOCATION="${CMAKE_INSTALL_PREFIX}/${VSXU_INSTALL_LIB_DIR}/vsxu/plugins")
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
  add_definitions(-DVSXU_PLUGIN_LOCATION="plugins")
endif()


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
  src/vsxfst/Common/StringConvert.cpp
  src/vsxfst/Common/Alloc.cpp
  src/vsxfst/Common/StringToInt.cpp
  src/vsxfst/Common/CRC.cpp
  src/vsxfst/Common/String.cpp
  src/vsxfst/Common/Vector.cpp
  src/vsxfst/Common/C_FileIO.cpp
  src/vsxfst/7zip/Common/OutBuffer.cpp
  src/vsxfst/7zip/Common/StreamUtils.cpp
  src/vsxfst/7zip/Common/InBuffer.cpp
  src/vsxfst/7zip/Compress/LZMA/LZMADecoder.cpp
  src/vsxfst/7zip/Compress/LZMA/LZMAEncoder.cpp
  src/vsxfst/7zip/Compress/LZ/LZOutWindow.cpp
  src/vsxfst/7zip/Compress/LZ/LZInWindow.cpp
  src/vsxfst/7zip/Compress/RangeCoder/RangeCoderBit.cpp
  src/vsxfst/LzmaRam.cpp
  src/vsxfst/vsxf.cpp
  src/vsxfst/vsxs.cpp
  src/core/vsx_engine.cpp
  src/core/vsx_engine_abs.cpp
  src/core/vsx_sequence_pool.cpp
  src/core/vsx_param_abstraction.cpp
  src/core/vsx_comp_channel.cpp
  src/core/vsx_param_interpolation.cpp
  src/core/vsx_comp/vsx_comp.cpp
  src/core/vsx_param_sequence.cpp
  src/core/vsx_master_sequencer/vsx_master_sequence_channel.cpp
  src/core/vsx_param_sequence_list.cpp
  src/core/vsx_module_static.cpp
  src/core/vsx_module_list/vsx_module_list_factory.cpp
  src/core/vsx_module_list/vsx_dlopen.cpp
  src/mtwist.c
  src/vsx_data_path.cpp
  src/vsx_argvector.cpp
  src/vsx_param.cpp
  src/vsx_sequence.cpp
  src/log/vsx_log.cpp
  src/vsx_command.cpp
  src/vsx_command_client_server.cpp
  src/vsxfst/7zip/Compress/LZMA_C/LzmaDecode.c
  src/vsxfst/7zip/Compress/Branch/BranchX86.c
  src/vsxfst/LzmaRamDecode.c
)

################################################################################
# LINKAGE ######################################################################
################################################################################

if (VSXU_STATIC EQUAL 1)

  #static build
  add_library(${module_id} STATIC ${SOURCES} ${HEADER_FILES} ${GAME_MONKEY})
  include(../plugins/static_factory_cmake_target_link_libraries)

else ()

  #dynamic build
  add_library(${module_id} SHARED ${SOURCES} ${HEADER_FILES} ${GAME_MONKEY})
endif ()

install(TARGETS ${module_id} DESTINATION ${VSXU_INSTALL_LIB_DIR} COMPONENT engine)

######################################################################################
# Install the Headers so as to install VSXu as a library
if (UNIX OR VSXU_WINDOWS_LIB_BUILD)
  file(GLOB HEADERS "include/*.h")
  install (FILES ${HEADERS} DESTINATION ${CMAKE_INSTALL_PREFIX}/${VSXU_INSTALL_INCLUDE_DIR}/engine COMPONENT Devel)
  install (DIRECTORY "include/container" DESTINATION ${CMAKE_INSTALL_PREFIX}/${VSXU_INSTALL_INCLUDE_DIR}/engine COMPONENT Devel)
  install (DIRECTORY "include/debug" DESTINATION ${CMAKE_INSTALL_PREFIX}/${VSXU_INSTALL_INCLUDE_DIR}/engine COMPONENT Devel)
  install (DIRECTORY "include/face" DESTINATION ${CMAKE_INSTALL_PREFIX}/${VSXU_INSTALL_INCLUDE_DIR}/engine COMPONENT Devel)
  install (DIRECTORY "include/vector" DESTINATION ${CMAKE_INSTALL_PREFIX}/${VSXU_INSTALL_INCLUDE_DIR}/engine COMPONENT Devel)
  install (DIRECTORY "include/string" DESTINATION ${CMAKE_INSTALL_PREFIX}/${VSXU_INSTALL_INCLUDE_DIR}/engine COMPONENT Devel)
  install (DIRECTORY "include/tools" DESTINATION ${CMAKE_INSTALL_PREFIX}/${VSXU_INSTALL_INCLUDE_DIR}/engine COMPONENT Devel)
endif()
################################################################################

if (WIN32)
  find_file(pthread_dll pthreadGC2.dll)
  install(FILES ${pthread_dll} DESTINATION ${VSXU_INSTALL_BIN_DIR} COMPONENT engine)
  if (VSXU_BUILD_WINDOWS_32)
    install(FILES /usr/lib/gcc/i686-w64-mingw32/4.6/libstdc++-6.dll DESTINATION ${VSXU_INSTALL_BIN_DIR} COMPONENT engine)
    install(FILES /usr/lib/gcc/i686-w64-mingw32/4.6/libgcc_s_sjlj-1.dll DESTINATION ${VSXU_INSTALL_BIN_DIR} COMPONENT engine)
  endif()
  if (VSXU_BUILD_WINDOWS_64)
    install(FILES /usr/lib/gcc/x86_64-w64-mingw32/4.6/libstdc++-6.dll DESTINATION ${VSXU_INSTALL_BIN_DIR} COMPONENT engine)
    install(FILES /usr/lib/gcc/x86_64-w64-mingw32/4.6/libgcc_s_sjlj-1.dll DESTINATION ${VSXU_INSTALL_BIN_DIR} COMPONENT engine)
  endif()
endif()

target_link_libraries(${module_id}
  ${DL_LIBRARY}
  ${CMAKE_THREAD_LIBS_INIT}
  ${PTHREAD_LIBRARY}
)
