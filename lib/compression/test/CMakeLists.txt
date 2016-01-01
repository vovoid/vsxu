cmake_minimum_required(VERSION 2.8)

  add_definitions(
    -g
    -Wtype-limits
    -Wuninitialized
    -Wignored-qualifiers
    -Wempty-body
    -Wunused-parameter
  )

if(WIN32)
  add_definitions( -Wno-error ) # vsx_filesystem::filesystemst WIN32
endif(WIN32)

include_directories(
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}/lib/common/include
  ${CMAKE_SOURCE_DIR}/lib/compression/include
)

add_executable(test_compression_lzma test_compression_lzma.cpp )
target_link_libraries(test_compression_lzma rt vsx_common vsx_compression)

add_executable(test_compression_lzma_old test_compression_lzma_old.cpp )
target_link_libraries(test_compression_lzma_old rt vsx_common vsx_compression)

