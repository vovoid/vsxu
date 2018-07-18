set(module_id vsx_widget)

message("configuring            " ${module_id})

################################################################################
project (${module_id})


include(${CMAKE_SOURCE_DIR}/cmake/CMakeFindLib.txt)

################################################################################
# CMAKE PACKAGES ###############################################################
################################################################################
find_package(Threads REQUIRED)
find_package(OpenGL REQUIRED)

################################################################################
# INCLUDES #####################################################################
################################################################################

include_directories(
  ${CMAKE_SOURCE_DIR}/
  ${CMAKE_SOURCE_DIR}/lib/common/include
  ${CMAKE_SOURCE_DIR}/lib/application/include
  ${CMAKE_SOURCE_DIR}/lib/engine/include
  ${CMAKE_SOURCE_DIR}/lib/engine_graphics/include
  include
  include/widgets
)



################################################################################
# DEFINES
################################################################################
# - if you want an engine without OpenGL linkage, add -DVSX_NO_GL
################################################################################

add_definitions(
 -DCMAKE_INSTALL_PREFIX="${CMAKE_INSTALL_PREFIX}"
)
if (MSVC)
else()
  add_definitions(
   -Werror=float-equal
  )
endif()

if (VSXU_DEBUG EQUAL 1)
  add_definitions( -DVSXU_DEBUG )
endif (VSXU_DEBUG EQUAL 1)

if (VSXU_STATIC EQUAL 1)
  add_definitions(-DVSXU_STATIC)
  add_definitions(-DGLEW_STATIC)
endif()


################################################################################
# SOURCES ######################################################################
################################################################################

file(GLOB_RECURSE SOURCES include/*.h)
file(GLOB_RECURSE SOURCES_CPP include/*.cpp)
set(SOURCES ${SOURCES} ${SOURCES_CPP})
file(GLOB_RECURSE SOURCES_CPP src/*.cpp)
set(SOURCES ${SOURCES} ${SOURCES_CPP})

################################################################################
# LINKAGE ######################################################################
################################################################################

if (VSXU_STATIC EQUAL 1)

  #static build
  add_library(${module_id} STATIC ${SOURCES})

else ()

  #dynamic build
  add_library(${module_id} SHARED ${SOURCES})

endif ()


if (UNIX)
  install(TARGETS ${module_id} DESTINATION ${VSXU_INSTALL_LIB_DIR} COMPONENT widget)

  ######################################################################################
  # Install the Headers so as to install VSXu as a library
  install (DIRECTORY "include/" DESTINATION ${CMAKE_INSTALL_PREFIX}/${VSXU_INSTALL_INCLUDE_DIR}/widget COMPONENT Devel)
  ################################################################################
endif()

if(UNIX)
target_link_libraries(
  ${module_id}
  ${JPEG_LIBRARIES}
  ${PNG_LIBRARIES}
  ${ZLIB_LIBRARIES}
  ${GLFW_LIBRARIES}
  ${GLEW_LIBRARIES}
  vsx_engine
  ${OPENGL_LIBRARIES}
  ${CMAKE_THREAD_LIBS_INIT}
)
endif(UNIX)

if(WIN32)
  target_link_libraries(
    ${module_id}
    ${JPEG_LIBRARIES}
    ${PNG_LIBRARIES}
    ${ZLIB_LIBRARIES}
    ${GLFW_LIBRARIES}
    vsx_engine
    vsx_engine_graphics
    ${GLEW_LIBRARIES}
    ${OPENGL_LIBRARIES}
    vsx_engine_graphics
  )
endif(WIN32)

