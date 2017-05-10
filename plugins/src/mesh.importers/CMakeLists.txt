cmake_minimum_required(VERSION 2.6)

# compute module id from compilation path
get_filename_component(list_file_path ${CMAKE_CURRENT_LIST_FILE} PATH)
string(REGEX MATCH "[a-z0-9._-]*$" module_id ${list_file_path})

message("configuring vsxu module            " ${module_id})

# set defines for MOD_CM, MOD_DM, MOD_NM
CREATE_STATIC_GLUE(${module_id})

if(UNIX)
  add_definitions( -fPIC -fpermissive )
endif(UNIX)

set(DEFINITIONS
  -DCAL3D_EXPORTS
)

if(NOT MSVC)
  set(DEFINITIONS ${DEFINITIONS}
    -Wno-narrowing
    -Wno-vla
    -Wno-parentheses
    -Wno-unused-but-set-variable
  )
endif()

set(INCLUDE_DIRS ${CMAKE_CURRENT_LIST_DIR}/cal3d/cal3d/src)

add_definitions(${DEFINITIONS})

include_directories(${INCLUDE_DIRS})

# find the local source files
file(GLOB LOCAL_SOURCES "*.cpp")

set(SOURCES
${CMAKE_CURRENT_LIST_DIR}/cal3d/cal3d/src/cal3d/animation_action.cpp
${CMAKE_CURRENT_LIST_DIR}/cal3d/cal3d/src/cal3d/animation.cpp
${CMAKE_CURRENT_LIST_DIR}/cal3d/cal3d/src/cal3d/animation_cycle.cpp
${CMAKE_CURRENT_LIST_DIR}/cal3d/cal3d/src/cal3d/bone.cpp
${CMAKE_CURRENT_LIST_DIR}/cal3d/cal3d/src/cal3d/buffersource.cpp
${CMAKE_CURRENT_LIST_DIR}/cal3d/cal3d/src/cal3d/cal3d_wrapper.cpp
#${CMAKE_CURRENT_LIST_DIR}/cal3d/cal3d/src/cal3d/calxmlbindings.cpp
${CMAKE_CURRENT_LIST_DIR}/cal3d/cal3d/src/cal3d/coreanimatedmorph.cpp
${CMAKE_CURRENT_LIST_DIR}/cal3d/cal3d/src/cal3d/coreanimation.cpp
${CMAKE_CURRENT_LIST_DIR}/cal3d/cal3d/src/cal3d/corebone.cpp
${CMAKE_CURRENT_LIST_DIR}/cal3d/cal3d/src/cal3d/corekeyframe.cpp
${CMAKE_CURRENT_LIST_DIR}/cal3d/cal3d/src/cal3d/corematerial.cpp
${CMAKE_CURRENT_LIST_DIR}/cal3d/cal3d/src/cal3d/coremesh.cpp
${CMAKE_CURRENT_LIST_DIR}/cal3d/cal3d/src/cal3d/coremodel.cpp
${CMAKE_CURRENT_LIST_DIR}/cal3d/cal3d/src/cal3d/coremorphkeyframe.cpp
${CMAKE_CURRENT_LIST_DIR}/cal3d/cal3d/src/cal3d/coremorphtrack.cpp
${CMAKE_CURRENT_LIST_DIR}/cal3d/cal3d/src/cal3d/coreskeleton.cpp
${CMAKE_CURRENT_LIST_DIR}/cal3d/cal3d/src/cal3d/coresubmesh.cpp
${CMAKE_CURRENT_LIST_DIR}/cal3d/cal3d/src/cal3d/coresubmorphtarget.cpp
${CMAKE_CURRENT_LIST_DIR}/cal3d/cal3d/src/cal3d/coretrack.cpp
${CMAKE_CURRENT_LIST_DIR}/cal3d/cal3d/src/cal3d/dualquaternion.cpp
${CMAKE_CURRENT_LIST_DIR}/cal3d/cal3d/src/cal3d/error.cpp
${CMAKE_CURRENT_LIST_DIR}/cal3d/cal3d/src/cal3d/global.cpp
${CMAKE_CURRENT_LIST_DIR}/cal3d/cal3d/src/cal3d/hardwaremodel.cpp
${CMAKE_CURRENT_LIST_DIR}/cal3d/cal3d/src/cal3d/loader.cpp
${CMAKE_CURRENT_LIST_DIR}/cal3d/cal3d/src/cal3d/matrix.cpp
${CMAKE_CURRENT_LIST_DIR}/cal3d/cal3d/src/cal3d/mesh.cpp
${CMAKE_CURRENT_LIST_DIR}/cal3d/cal3d/src/cal3d/mixer.cpp
${CMAKE_CURRENT_LIST_DIR}/cal3d/cal3d/src/cal3d/model.cpp
${CMAKE_CURRENT_LIST_DIR}/cal3d/cal3d/src/cal3d/morphtargetmixer.cpp
${CMAKE_CURRENT_LIST_DIR}/cal3d/cal3d/src/cal3d/physique.cpp
${CMAKE_CURRENT_LIST_DIR}/cal3d/cal3d/src/cal3d/physiquedualquaternion.cpp
${CMAKE_CURRENT_LIST_DIR}/cal3d/cal3d/src/cal3d/platform.cpp
${CMAKE_CURRENT_LIST_DIR}/cal3d/cal3d/src/cal3d/quaternion.cpp
${CMAKE_CURRENT_LIST_DIR}/cal3d/cal3d/src/cal3d/renderer.cpp
${CMAKE_CURRENT_LIST_DIR}/cal3d/cal3d/src/cal3d/saver.cpp
${CMAKE_CURRENT_LIST_DIR}/cal3d/cal3d/src/cal3d/skeleton.cpp
${CMAKE_CURRENT_LIST_DIR}/cal3d/cal3d/src/cal3d/springsystem.cpp
${CMAKE_CURRENT_LIST_DIR}/cal3d/cal3d/src/cal3d/streamsource.cpp
${CMAKE_CURRENT_LIST_DIR}/cal3d/cal3d/src/cal3d/submesh.cpp
${CMAKE_CURRENT_LIST_DIR}/cal3d/cal3d/src/cal3d/tinybind.cpp
${CMAKE_CURRENT_LIST_DIR}/cal3d/cal3d/src/cal3d/tinyxml.cpp
${CMAKE_CURRENT_LIST_DIR}/cal3d/cal3d/src/cal3d/tinyxmlerror.cpp
${CMAKE_CURRENT_LIST_DIR}/cal3d/cal3d/src/cal3d/tinyxmlparser.cpp
${CMAKE_CURRENT_LIST_DIR}/cal3d/cal3d/src/cal3d/vector.cpp
${CMAKE_CURRENT_LIST_DIR}/cal3d/cal3d/src/cal3d/xmlformat.cpp
)

file(GLOB_RECURSE HEADER_FILES *.h)

# include common
include(../cmake_globals.txt)

# actual compilation target
CREATE_MODULE(${module_id})

# include common
include(../cmake_suffix.txt)

