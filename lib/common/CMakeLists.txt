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
)

file(GLOB_RECURSE SOURCES_HEADERS include/*.h)
set(SOURCES ${SOURCES} ${SOURCES_HEADERS} )

file(GLOB_RECURSE SOURCES_HEADERS src/*.cpp)
set(SOURCES ${SOURCES} ${SOURCES_HEADERS} )

file(GLOB_RECURSE SOURCES_HEADERS include/*.cpp)
set(SOURCES ${SOURCES} ${SOURCES_HEADERS} )

if (NOT WIN32)
  set(SOURCES ${SOURCES} src/command/vsx_command_client_server.cpp)
endif()

################################################################################
# LINKAGE ######################################################################
################################################################################

if (VSXU_STATIC EQUAL 1)
  add_library(${module_id} STATIC ${SOURCES} ${HEADER_FILES})
else ()
  add_library(${module_id} SHARED ${SOURCES} ${HEADER_FILES})
endif ()

target_link_libraries(${module_id} ${CMAKE_THREAD_LIBS_INIT} vsx_compression)

install(TARGETS ${module_id} DESTINATION ${VSXU_INSTALL_LIB_DIR} COMPONENT common)

######################################################################################
# Install the Headers so as to install VSXu as a library
install (DIRECTORY "include/" DESTINATION ${CMAKE_INSTALL_PREFIX}/${VSXU_INSTALL_INCLUDE_DIR}/common COMPONENT Devel)
################################################################################

add_subdirectory(test)
