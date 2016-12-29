cmake_minimum_required(VERSION 2.8)

include(${CMAKE_SOURCE_DIR}/cmake/CMakeFindLib.txt)

find_package(Threads REQUIRED)

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})

include(${CMAKE_SOURCE_DIR}/cmake/CMakeVSXuGfxLib.txt)

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
  ${CMAKE_SOURCE_DIR}/lib/engine_graphics/include
  ${CMAKE_SOURCE_DIR}/lib/common/include
)

add_executable(test_input test_input.cpp )
target_link_libraries(test_input
  ${OPENGL_LIBRARIES}
  vsx_application
  ${SDL2_LIBRARIES}
)
if (NOT WIN32)
    target_link_libraries(test_input rt )
endif()
target_link_libraries(test_input
  vsx_common
  vsx_engine_graphics
  vsx_compression
  ${CMAKE_THREAD_LIBS_INIT}
)
