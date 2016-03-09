set(module_id vsx_common)

message("configuring            " ${module_id})


################################################################################
project (${module_id})

################################################################################
# CMAKE PACKAGES ###############################################################
################################################################################
find_package(DL REQUIRED)
find_package(Threads REQUIRED)

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
  third-party/mt/mt.cc
  src/vsx_argvector.cpp
  src/math/vsx_rand.cpp
  src/audio/vsx_audio_mixer_manager.cpp
  src/vsx_data_path.cpp
  src/log/vsx_log.cpp
  src/filesystem/archive/vsx_filesystem_archive_reader.cpp
  src/filesystem/archive/vsx/vsx_filesystem_archive_vsx_reader.cpp
  src/filesystem/archive/vsx/vsx_filesystem_archive_vsx_writer.cpp
  src/filesystem/archive/vsxz/vsx_filesystem_archive_vsxz_reader.cpp
  src/filesystem/archive/vsxz/vsx_filesystem_archive_vsxz_writer.cpp
  src/filesystem/vsx_filesystem_helper.cpp
  src/filesystem/vsx_filesystem.cpp
)

################################################################################
# LINKAGE ######################################################################
################################################################################

if (VSXU_STATIC EQUAL 1)
  add_library(${module_id} STATIC ${SOURCES} ${HEADER_FILES})
else ()
  add_library(${module_id} SHARED ${SOURCES} ${HEADER_FILES})
endif ()

target_link_libraries(${module_id} ${CMAKE_THREAD_LIBS_INIT})

install(TARGETS ${module_id} DESTINATION ${VSXU_INSTALL_LIB_DIR} COMPONENT common)

######################################################################################
# Install the Headers so as to install VSXu as a library
if (UNIX OR VSXU_WINDOWS_LIB_BUILD)
  file(GLOB HEADERS "include/*.h")
  install (DIRECTORY "include/audio" DESTINATION ${CMAKE_INSTALL_PREFIX}/${VSXU_INSTALL_INCLUDE_DIR}/engine COMPONENT Devel)
  install (DIRECTORY "include/color" DESTINATION ${CMAKE_INSTALL_PREFIX}/${VSXU_INSTALL_INCLUDE_DIR}/engine COMPONENT Devel)
  install (DIRECTORY "include/container" DESTINATION ${CMAKE_INSTALL_PREFIX}/${VSXU_INSTALL_INCLUDE_DIR}/engine COMPONENT Devel)
  install (DIRECTORY "include/debug" DESTINATION ${CMAKE_INSTALL_PREFIX}/${VSXU_INSTALL_INCLUDE_DIR}/engine COMPONENT Devel)
  install (DIRECTORY "include/filesystem" DESTINATION ${CMAKE_INSTALL_PREFIX}/${VSXU_INSTALL_INCLUDE_DIR}/engine COMPONENT Devel)
  install (DIRECTORY "include/graphics" DESTINATION ${CMAKE_INSTALL_PREFIX}/${VSXU_INSTALL_INCLUDE_DIR}/common COMPONENT Devel)
  install (DIRECTORY "include/log" DESTINATION ${CMAKE_INSTALL_PREFIX}/${VSXU_INSTALL_INCLUDE_DIR}/common COMPONENT Devel)
  install (DIRECTORY "include/math" DESTINATION ${CMAKE_INSTALL_PREFIX}/${VSXU_INSTALL_INCLUDE_DIR}/common COMPONENT Devel)
  install (DIRECTORY "include/particlesystem" DESTINATION ${CMAKE_INSTALL_PREFIX}/${VSXU_INSTALL_INCLUDE_DIR}/common COMPONENT Devel)
  install (DIRECTORY "include/profiler" DESTINATION ${CMAKE_INSTALL_PREFIX}/${VSXU_INSTALL_INCLUDE_DIR}/engine COMPONENT Devel)
  install (DIRECTORY "include/string" DESTINATION ${CMAKE_INSTALL_PREFIX}/${VSXU_INSTALL_INCLUDE_DIR}/engine COMPONENT Devel)
  install (DIRECTORY "include/time" DESTINATION ${CMAKE_INSTALL_PREFIX}/${VSXU_INSTALL_INCLUDE_DIR}/engine COMPONENT Devel)
  install (DIRECTORY "include/tools" DESTINATION ${CMAKE_INSTALL_PREFIX}/${VSXU_INSTALL_INCLUDE_DIR}/engine COMPONENT Devel)
endif()
################################################################################


if (WIN32)
  if (VSXU_BUILD_WINDOWS_32)
    install(FILES  DESTINATION ${VSXU_INSTALL_BIN_DIR} COMPONENT common)
  endif()
endif()

add_subdirectory(test)
