cmake_minimum_required(VERSION 2.6)
if (PLATFORM_SHARED_FILES_FLAT_INSTALL EQUAL 1)
  add_definitions( -DPLATFORM_SHARED_FILES_FLAT_INSTALL )
endif (PLATFORM_SHARED_FILES_FLAT_INSTALL EQUAL 1)
  
add_subdirectory(engine)
add_subdirectory(engine_graphics)
add_subdirectory(plugins)

if (NOT VSXU_OPENGL_ES_2_0 EQUAL 1)
  add_subdirectory(player)
  add_subdirectory(artiste)
endif (NOT VSXU_OPENGL_ES_2_0 EQUAL 1)

add_subdirectory(server)

if (NOT VSXU_ENGINE_STATIC EQUAL 1)
  add_subdirectory(tools/vsxz)
endif (NOT VSXU_ENGINE_STATIC EQUAL 1)

if (VSXU_PLAYER_DEMO EQUAL 1)
add_subdirectory(player_demo)
endif (VSXU_PLAYER_DEMO EQUAL 1)
