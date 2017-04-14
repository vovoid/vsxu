set(module_id vsx_compression)

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
  ${CMAKE_SOURCE_DIR}/lib/compression/include
)

file(GLOB_RECURSE HEADER_FILES *.h)

################################################################################
# DEFINES
################################################################################

add_definitions(
 -DCMAKE_INSTALL_PREFIX="${CMAKE_INSTALL_PREFIX}"
) 
if (MSVC)
else()
  add_definitions(-fno-strict-aliasing)
endif()

if (VSXU_DEBUG EQUAL 1)
  add_definitions( -DVSXU_DEBUG )
endif (VSXU_DEBUG EQUAL 1)


################################################################################
# SOURCES ######################################################################
################################################################################

set(SOURCES
  src/vsx_compression_lzma.cpp
  src/vsx_compression_lzma_old.cpp
  src/vsx_compression_lzham.cpp
)


add_definitions(-D_7ZIP_ST)
set(SOURCES_LZMA
  thirdparty/lzma-sdk/C/Alloc.c
  thirdparty/lzma-sdk/C/LzmaLib.c
  thirdparty/lzma-sdk/C/LzmaEnc.c
  thirdparty/lzma-sdk/C/LzmaDec.c
  thirdparty/lzma-sdk/C/LzFind.c
)


add_definitions(
  -DLZHAM_NO_THREADING=1
  -fno-strict-aliasing
)
include_directories(
  ${CMAKE_SOURCE_DIR}/lib/compression/thirdparty/lzham-sdk/include
  ${CMAKE_SOURCE_DIR}/lib/compression/thirdparty/lzham-sdk/lzhamcomp
  ${CMAKE_SOURCE_DIR}/lib/compression/thirdparty/lzham-sdk/lzhamdecomp
)
set(SOURCES_LZHAM
  thirdparty/lzham-sdk/lzhamlib/lzham_lib.cpp
  thirdparty/lzham-sdk/lzhamdecomp/lzham_assert.cpp
  thirdparty/lzham-sdk/lzhamdecomp/lzham_checksum.cpp
  thirdparty/lzham-sdk/lzhamdecomp/lzham_huffman_codes.cpp
  thirdparty/lzham-sdk/lzhamdecomp/lzham_lzdecompbase.cpp
  thirdparty/lzham-sdk/lzhamdecomp/lzham_lzdecomp.cpp
  thirdparty/lzham-sdk/lzhamdecomp/lzham_mem.cpp
  thirdparty/lzham-sdk/lzhamdecomp/lzham_platform.cpp
  thirdparty/lzham-sdk/lzhamdecomp/lzham_prefix_coding.cpp
  thirdparty/lzham-sdk/lzhamdecomp/lzham_symbol_codec.cpp
  #thirdparty/lzham-sdk/lzhamdecomp/lzham_timer.cpp
  thirdparty/lzham-sdk/lzhamdecomp/lzham_vector.cpp
  thirdparty/lzham-sdk/lzhamcomp/lzham_lzbase.cpp
  thirdparty/lzham-sdk/lzhamcomp/lzham_lzcomp.cpp
  thirdparty/lzham-sdk/lzhamcomp/lzham_lzcomp_internal.cpp
  thirdparty/lzham-sdk/lzhamcomp/lzham_lzcomp_state.cpp
  thirdparty/lzham-sdk/lzhamcomp/lzham_match_accel.cpp
)

if (WIN32)
    set(SOURCES_LZMA_OLD
        thirdparty/lzma-old/7zip/Common/InBuffer.cpp
        thirdparty/lzma-old/7zip/Common/OutBuffer.cpp
        thirdparty/lzma-old/7zip/Common/StreamUtils.cpp
        thirdparty/lzma-old/7zip/Compress/Branch/BranchX86.c
        thirdparty/lzma-old/7zip/Compress/LZMA/LZMADecoder.cpp
        thirdparty/lzma-old/7zip/Compress/LZMA/LZMAEncoder.cpp
        thirdparty/lzma-old/7zip/Compress/LZMA_C/LzmaDecode.c
        thirdparty/lzma-old/7zip/Compress/LZ/LZInWindow.cpp
        thirdparty/lzma-old/7zip/Compress/LZ/LZOutWindow.cpp
        thirdparty/lzma-old/7zip/Compress/RangeCoder/RangeCoderBit.cpp
        thirdparty/lzma-old/Common/Alloc_.cpp
        thirdparty/lzma-old/Common/CRC.cpp
        thirdparty/lzma-old/Common/String.cpp
        thirdparty/lzma-old/Common/StringConvert.cpp
        thirdparty/lzma-old/Common/StringToInt.cpp
        thirdparty/lzma-old/Common/Vector.cpp
        thirdparty/lzma-old/LzmaRam.cpp
        thirdparty/lzma-old/LzmaRamDecode.c
     )
else()
    set(SOURCES_LZMA_OLD
      thirdparty/lzma-old/Common/StringConvert.cpp
      thirdparty/lzma-old/Common/Alloc_.cpp
      thirdparty/lzma-old/Common/StringToInt.cpp
      thirdparty/lzma-old/Common/CRC.cpp
      thirdparty/lzma-old/Common/String.cpp
      thirdparty/lzma-old/Common/Vector.cpp
      thirdparty/lzma-old/Common/C_FileIO.cpp
      thirdparty/lzma-old/7zip/Common/OutBuffer.cpp
      thirdparty/lzma-old/7zip/Common/StreamUtils.cpp
      thirdparty/lzma-old/7zip/Common/InBuffer.cpp
      thirdparty/lzma-old/7zip/Compress/LZMA/LZMADecoder.cpp
      thirdparty/lzma-old/7zip/Compress/LZMA/LZMAEncoder.cpp
      thirdparty/lzma-old/7zip/Compress/LZ/LZOutWindow.cpp
      thirdparty/lzma-old/7zip/Compress/LZ/LZInWindow.cpp
      thirdparty/lzma-old/7zip/Compress/RangeCoder/RangeCoderBit.cpp
      thirdparty/lzma-old/LzmaRam.cpp
      thirdparty/lzma-old/7zip/Compress/LZMA_C/LzmaDecode.c
      thirdparty/lzma-old/7zip/Compress/Branch/BranchX86.c
      thirdparty/lzma-old/LzmaRamDecode.c
    )
endif()


################################################################################
# LINKAGE ######################################################################
################################################################################

if (VSXU_STATIC EQUAL 1)
  add_library(${module_id} STATIC ${SOURCES} ${SOURCES_LZMA} ${SOURCES_LZHAM} ${SOURCES_LZMA_OLD} ${HEADER_FILES})
else ()
  add_library(${module_id} SHARED ${SOURCES} ${SOURCES_LZMA} ${SOURCES_LZHAM} ${SOURCES_LZMA_OLD} ${HEADER_FILES})
endif ()

install(TARGETS ${module_id} DESTINATION ${VSXU_INSTALL_LIB_DIR} COMPONENT common)

######################################################################################
# Install the Headers so as to install VSXu as a library
install (DIRECTORY "include/" DESTINATION ${CMAKE_INSTALL_PREFIX}/${VSXU_INSTALL_INCLUDE_DIR}/application COMPONENT Devel)
################################################################################

if (WIN32)
  if (VSXU_BUILD_WINDOWS_32)
    install(FILES  DESTINATION ${VSXU_INSTALL_BIN_DIR} COMPONENT common)
  endif()
endif()

add_subdirectory(test)
