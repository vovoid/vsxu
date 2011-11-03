cmake_minimum_required(VERSION 2.8)

PROJECT(VSXu)


SET(CPACK_PACKAGE_NAME "VSXu")
SET(CPACK_PACKAGE_VERSION "0.3.0")

if (UNIX)
SET(CPACK_PACKAGING_INSTALL_PREFIX "/opt/vovoid")
SET(CPACK_SET_DESTDIR "on")
set(CPACK_PACKAGING_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
endif(UNIX)

SET(VERSION 0.3.0)
SET($ {VERSION} CPACK_DEBIAN_PACKAGE_VERSION)
SET(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
SET(CPACK_DEBIAN_PACKAGE_SECTION "graphics")
SET(CPACK_DEBIAN_ARCHITECTURE ${CMAKE_SYSTEM_PROCESSOR})
SET(CPACK_DEBIAN_PACKAGE_DEPENDS "libxrandr2 (> = 2: 1.3.0-3), libc6 (> = 2.1.3), libgcc1 (> = 1: 4.1.1)")

SET(CPACK_PACKAGE_DESCRIPTION "Music Visualizer")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Music Visualizer, Realtime Graphics Visual Programming Language, ")
SET(CPACK_PACKAGE_CONTACT "Jonatan Wallmander <jonatan@vovoid.com> ")
SET(CPACK_PACKAGE_FILE_NAME ${CMAKE_PROJECT_NAME}_${VERSION}_${CPACK_DEBIAN_ARCHITECTURE})

SET(CPACK_COMPONENTS_ALL Applications Libraries ApplicationData)


SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "Jonatan 'jaw' Wallmander") #required

if (PLATFORM_SHARED_FILES_FLAT_INSTALL EQUAL 1)
  add_definitions( -DPLATFORM_SHARED_FILES_FLAT_INSTALL )
endif (PLATFORM_SHARED_FILES_FLAT_INSTALL EQUAL 1)

set (CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules)

if(VSXU_DEBUG)
  add_definitions(-g -Wall)
else(VSXU_DEBUG)
  if (OPTIMIZATION_FLAGS EQUAL 1)
    add_definitions(-lrt -lm -O1 -march=opteron -fno-thread-jumps -fno-if-conversion2 -fno-delayed-branch -fno-loop-optimize -ftree-dce -ftree-sra -ftree-copyrename -ftree-fre -ftree-ch -fmerge-constants -fcrossjumping -fcse-follow-jumps -fcse-skip-blocks -fexpensive-optimizations -fstrength-reduce -frerun-cse-after-loop -frerun-loop-opt -fforce-addr -fpeephole2 -fschedule-insns2 -fregmove -freorder-blocks -fsched-interblock -funit-at-a-time -falign-functions -falign-jumps -falign-loops -falign-labels -finline-functions -fno-inline -ftracer -fmodulo-sched -fgcse-sm -freschedule-modulo-scheduled-loops -ftree-loop-im -ftree-loop-ivcanon -maccumulate-outgoing-args -mno-align-stringops -D__NO_MATH_INLINES -funsafe-math-optimizations -fno-signaling-nans)
  else (OPTIMIZATION_FLAGS EQUAL 1)
    add_definitions(-Os -c -fmessage-length=0 -finline-functions)
  endif (OPTIMIZATION_FLAGS EQUAL 1)
endif(VSXU_DEBUG)

#add_definitions(-Os -c -fmessage-length=0 -finline-functions)
#add_definitions(-O3 -c -fmessage-length=0 -finline-functions)


#find_package(DL REQUIRED)
#find_package(GLEW REQUIRED)
#find_package(GLFW REQUIRED)
#find_package(PNG REQUIRED)
#find_package(JPEG REQUIRED)
#find_package(FTGL REQUIRED)
#find_package(PULSEAUDIO REQUIRED)
#find_package(Threads REQUIRED)
#find_package(X11 COMPONENTS X11_Xrandr REQUIRED)

#if(NOT X11_Xrandr_FOUND)
  #message("ERROR: Xrandr lib not found, terminating")
  #return(10)
#endif(NOT X11_Xrandr_FOUND)


################################################################################
# VSXU LIBRARIES ###############################################################
################################################################################
add_subdirectory(engine)
add_subdirectory(engine_graphics)
add_subdirectory(plugins)


################################################################################
# VSXU IMPLEMENTATIONS/BINARIES ################################################
################################################################################
if (NOT VSXU_OPENGL_ES_2_0 EQUAL 1)
  add_subdirectory(artiste)
  add_subdirectory(player)
endif (NOT VSXU_OPENGL_ES_2_0 EQUAL 1)

if(UNIX)
  add_subdirectory(server)
endif(UNIX)

if (NOT VSXU_ENGINE_STATIC EQUAL 1)
  add_subdirectory(tools/vsxz)
endif (NOT VSXU_ENGINE_STATIC EQUAL 1)

if (VSXU_PLAYER_DEMO EQUAL 1)
  add_subdirectory(player_demo)
endif (VSXU_PLAYER_DEMO EQUAL 1)

################################################################################
# CPACK ########################################################################
################################################################################
INCLUDE(CPack)
