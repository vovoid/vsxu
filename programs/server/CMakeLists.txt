set(module_id vsxu_server)

message("configuring            " ${module_id})


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
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}/lib/common/include
  ${CMAKE_SOURCE_DIR}/lib/engine/include
  ${CMAKE_SOURCE_DIR}/lib/engine_graphics/include
  src
)


################################################################################
# DEFINES ######################################################################
################################################################################

if(VSXU_DEBUG)
  add_definitions( -DDEBUG )
  add_definitions( -DVSXU_DEBUG )
endif(VSXU_DEBUG)

add_definitions(
 -DVSXU_ARTISTE
 -DVSX_FONT_NO_FT
 -DCMAKE_INSTALL_PREFIX="${CMAKE_INSTALL_PREFIX}"
)

if (VSXU_STATIC EQUAL 1)
  add_definitions(-DVSXU_STATIC)
endif()


################################################################################
# SOURCES ######################################################################
################################################################################

set(OS_SOURCES "")

if(UNIX)
  if (VSXU_OPENGL_ES_2_0 EQUAL 1)
    if (MEEGO EQUAL 1)
      include_directories(src/main/linux-opengl/)
      add_definitions(-DVSXU_OPENGL_ES)
      set(OS_SOURCES
        src/main/linux-opengl_es/main.cpp
      )
    else (VSXU_MEEGO EQUAL 1)
      include_directories(src/main/linux-opengl/)
      add_definitions(-DVSXU_OPENGL_ES)
      set(OS_SOURCES
        src/main/linux-meego_opengl_es/main.cpp
      )
    endif (MEEGO EQUAL 1)
  else (VSXU_OPENGL_ES_2_0 EQUAL 1)
    set(OS_SOURCES
      src/main/linux-opengl/main.cpp
    )
  endif (VSXU_OPENGL_ES_2_0 EQUAL 1)

endif(UNIX)



set(SOURCES
  src/application.cpp
)

if (VSXU_STATIC EQUAL 1)
  include(../plugins/static_factory_cmake_link_directories)
endif (VSXU_STATIC EQUAL 1)


if(UNIX)

  add_executable(${module_id}  ${SOURCES} ${OS_SOURCES})
  include(${CMAKE_SOURCE_DIR}/cmake_suffix.txt)

  target_link_libraries(${module_id}
    ${GLFW_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
    ${GLEW_LIBRARIES}
    ${PNG_LIBRARIES}
    ${JPEG_LIBRARIES}
    rt
  )


  target_link_libraries(${module_id} vsx_common)
  target_link_libraries(${module_id} vsx_compression)
  target_link_libraries(${module_id} vsx_engine)
  target_link_libraries(${module_id} vsx_engine_graphics)

  if (VSXU_STATIC EQUAL 1)
    include(../plugins/static_factory_cmake_target_link_libraries)
  endif ()

  install(TARGETS ${module_id} DESTINATION ${VSXU_INSTALL_BIN_DIR} COMPONENT server)
endif(UNIX)

IF (UNIX)
  CONFIGURE_FILE (${CMAKE_CURRENT_SOURCE_DIR}/vsxu-server-fullscreen.desktop.in
                  ${CMAKE_CURRENT_BINARY_DIR}/vsxu-server-fullscreen.desktop
                  @ONLY)
  CONFIGURE_FILE (${CMAKE_CURRENT_SOURCE_DIR}/vsxu-server.desktop.in
                  ${CMAKE_CURRENT_BINARY_DIR}/vsxu-server.desktop
                  @ONLY)

  INSTALL (FILES ${CMAKE_CURRENT_BINARY_DIR}/vsxu-server-fullscreen.desktop
                 ${CMAKE_CURRENT_BINARY_DIR}/vsxu-server.desktop
           DESTINATION ${CMAKE_INSTALL_PREFIX}/share/applications/)
ENDIF (UNIX)
