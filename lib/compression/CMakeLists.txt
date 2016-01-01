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
  third-party/lzma-sdk/C/Alloc.c
  third-party/lzma-sdk/C/LzmaLib.c
  third-party/lzma-sdk/C/LzmaEnc.c
  third-party/lzma-sdk/C/LzmaDec.c
  third-party/lzma-sdk/C/LzFind.c
)

set(SOURCES_LZMA_OLD
#  third-party/lzma-old/Common/StringConvert.cpp
  third-party/lzma-old/Common/StringConvert.cpp
  third-party/lzma-old/Common/Alloc.cpp
  third-party/lzma-old/Common/StringToInt.cpp
  third-party/lzma-old/Common/CRC.cpp
  third-party/lzma-old/Common/String.cpp
  third-party/lzma-old/Common/Vector.cpp
  third-party/lzma-old/Common/C_FileIO.cpp
  third-party/lzma-old/7zip/Common/OutBuffer.cpp
  third-party/lzma-old/7zip/Common/StreamUtils.cpp
  third-party/lzma-old/7zip/Common/InBuffer.cpp
  third-party/lzma-old/7zip/Compress/LZMA/LZMADecoder.cpp
  third-party/lzma-old/7zip/Compress/LZMA/LZMAEncoder.cpp
  third-party/lzma-old/7zip/Compress/LZ/LZOutWindow.cpp
  third-party/lzma-old/7zip/Compress/LZ/LZInWindow.cpp
  third-party/lzma-old/7zip/Compress/RangeCoder/RangeCoderBit.cpp
  third-party/lzma-old/LzmaRam.cpp
  third-party/lzma-old/7zip/Compress/LZMA_C/LzmaDecode.c
  third-party/lzma-old/7zip/Compress/Branch/BranchX86.c
  third-party/lzma-old/LzmaRamDecode.c
)


################################################################################
# LINKAGE ######################################################################
################################################################################

if (VSXU_STATIC EQUAL 1)
  add_library(${module_id} STATIC ${SOURCES} ${SOURCES_LZMA} ${SOURCES_LZMA_OLD} ${HEADER_FILES})
else ()
  add_library(${module_id} SHARED ${SOURCES} ${SOURCES_LZMA} ${SOURCES_LZMA_OLD} ${HEADER_FILES})
endif ()

install(TARGETS ${module_id} DESTINATION ${VSXU_INSTALL_LIB_DIR} COMPONENT common)

######################################################################################
# Install the Headers so as to install VSXu as a library
if (UNIX OR VSXU_WINDOWS_LIB_BUILD)
  file(GLOB HEADERS "include/*.h")
endif()
################################################################################

if (WIN32)
  if (VSXU_BUILD_WINDOWS_32)
    install(FILES  DESTINATION ${VSXU_INSTALL_BIN_DIR} COMPONENT common)
  endif()
endif()

add_subdirectory(test)
