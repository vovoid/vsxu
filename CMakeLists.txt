cmake_minimum_required(VERSION 2.6)
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

