cmake_minimum_required(VERSION 2.6)
include_directories(
../
../engine_graphics/include
include 
src/log
src/core/vsx_comp
src/core
)


if (VSXU_ENGINE_STATIC EQUAL 1)
  #static build
  include(../plugins/static_factory_cmake_link_directories)
  add_definitions(
    -DVSXU_ENGINE_STATIC
  )
endif (VSXU_ENGINE_STATIC EQUAL 1)

#definitions
#if you want an engine without OpenGL linkage, add -DVSX_NO_GL
add_definitions(
# to debug the string library, enable this
# -DVSXS_DEBUG=1
 -DVSX_ENG_DLL=1
 -DVSXU_MODULE_TIMING
 -DSAVE_PRODUCTION
 -DVSX_NOGLUT
 -DVSXU_ARTISTE
 -DVSXU_EXE
 -DVSX_TEXTURE_NO_RT
 -DVSX_TEXTURE_NO_JPEG
 -DVSXU_DEVELOPER
 -DCMAKE_INSTALL_PREFIX="${CMAKE_INSTALL_PREFIX}"
)

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

if (VSXU_DEBUG EQUAL 1)
  add_definitions( -DVSXU_DEBUG )
endif (VSXU_DEBUG EQUAL 1)


if (PLATFORM_SHARED_FILES_FLAT_INSTALL EQUAL 1)
  add_definitions( -DPLATFORM_SHARED_FILES_FLAT_INSTALL )
endif (PLATFORM_SHARED_FILES_FLAT_INSTALL EQUAL 1)


set(module_id vsxu_engine)
message("*******************************************")
message("configuring            " ${module_id})
message("*******************************************")

set(SOURCES
./src/vsxfst/Common/StringConvert.cpp
./src/vsxfst/Common/Alloc.cpp
./src/vsxfst/Common/StringToInt.cpp
./src/vsxfst/Common/CRC.cpp
./src/vsxfst/Common/String.cpp
./src/vsxfst/Common/Vector.cpp
./src/vsxfst/Common/C_FileIO.cpp
./src/vsxfst/7zip/Common/OutBuffer.cpp
./src/vsxfst/7zip/Common/StreamUtils.cpp
./src/vsxfst/7zip/Common/InBuffer.cpp
./src/vsxfst/7zip/Compress/LZMA/LZMADecoder.cpp
./src/vsxfst/7zip/Compress/LZMA/LZMAEncoder.cpp
./src/vsxfst/7zip/Compress/LZ/LZOutWindow.cpp
./src/vsxfst/7zip/Compress/LZ/LZInWindow.cpp
./src/vsxfst/7zip/Compress/RangeCoder/RangeCoderBit.cpp
./src/vsxfst/LzmaRam.cpp
./src/vsxfst/vsxf.cpp
./src/vsxfst/vsxs.cpp
./src/core/vsx_engine.cpp
./src/core/vsx_sequence_pool.cpp
./src/core/vsx_param_abstraction.cpp
./src/core/vsx_comp_channel.cpp
./src/core/vsx_param_interpolation.cpp
./src/core/vsx_comp/vsx_comp.cpp
./src/core/vsx_param_sequence.cpp
./src/core/vsx_master_sequencer/vsx_master_sequence_channel.cpp
./src/core/vsx_param_sequence_list.cpp
./src/core/vsx_module_static.cpp
./src/vsx_math_3d.cpp
./src/vsx_param.cpp
./src/vsx_sequence.cpp
./src/log/vsx_log.cpp
./src/vsx_command.cpp
./src/vsx_command_client_server.cpp
./src/vsxfst/7zip/Compress/LZMA_C/LzmaDecode.c
./src/vsxfst/7zip/Compress/Branch/BranchX86.c
./src/vsxfst/LzmaRamDecode.c
)
include(../cmake_globals.txt)
#link_directories(
#../vsxfst
#../vsxu_engine_gm
#)

set(STATIC_SOURCES
./src/core/vsx_module_static_factory/vsx_module_static_factory.cpp
)
project (${module_id})

if (VSXU_ENGINE_STATIC EQUAL 1)
  #static build
  
  add_library(${module_id}_static STATIC ${STATIC_SOURCES} ${SOURCES} ${GAME_MONKEY} ${EXTRA_LIBS})
  add_library(${module_id} SHARED ${STATIC_SOURCES} ${SOURCES} ${GAME_MONKEY} ${EXTRA_LIBS})
  include(../plugins/static_factory_cmake_target_link_libraries)
  install(TARGETS ${module_id} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
else (VSXU_ENGINE_STATIC EQUAL 1)
  #dynamic build
  add_library(${module_id} SHARED ${SOURCES} ${GAME_MONKEY} ${EXTRA_LIBS})
  install(TARGETS ${module_id} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
endif (VSXU_ENGINE_STATIC EQUAL 1)

#include(../cmake_suffix.txt)

