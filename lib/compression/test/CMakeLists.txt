cmake_minimum_required(VERSION 2.8)

if (NOT MSVC)
  add_definitions(
    -g
    -Wtype-limits
    -Wuninitialized
    -Wignored-qualifiers
    -Wempty-body
    -Wunused-parameter
  )
endif()

include_directories(
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}/lib/common/include
  ${CMAKE_SOURCE_DIR}/lib/compression/include
)

if (NOT WIN32)
    set(RT_LIBRARY rt)
else()
    set(RT_LIBRARY "")
endif()


add_executable(test_compression_lzma test_compression_lzma.cpp )
target_link_libraries(test_compression_lzma ${RT_LIBRARY} vsx_common vsx_compression)

add_executable(test_compression_lzham test_compression_lzham.cpp )
target_link_libraries(test_compression_lzham ${RT_LIBRARY} vsx_common vsx_compression)

add_executable(test_compression_lzma_old test_compression_lzma_old.cpp )
target_link_libraries(test_compression_lzma_old ${RT_LIBRARY} vsx_common vsx_compression)

