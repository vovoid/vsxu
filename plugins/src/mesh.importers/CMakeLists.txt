cmake_minimum_required(VERSION 2.6)

# compute module id from compilation path
get_filename_component(list_file_path ${CMAKE_CURRENT_LIST_FILE} PATH)
string(REGEX MATCH "[a-z0-9.-]*$" module_id ${list_file_path})

message("configuring vsxu module            " ${module_id})

# set defines for MOD_CM, MOD_DM, MOD_NM
CREATE_STATIC_GLUE(${module_id})

add_definitions(-DCAL3D_EXPORTS -fPIC)
include_directories(cal3d/cal3d/src)

# find the local source files
file(GLOB LOCAL_SOURCES "*.cpp")

set(SOURCES
cal3d/cal3d/src/cal3d/animation_action.cpp
cal3d/cal3d/src/cal3d/animation.cpp
cal3d/cal3d/src/cal3d/animation_cycle.cpp
cal3d/cal3d/src/cal3d/bone.cpp
cal3d/cal3d/src/cal3d/buffersource.cpp
cal3d/cal3d/src/cal3d/cal3d_wrapper.cpp
cal3d/cal3d/src/cal3d/calxmlbindings.cpp
cal3d/cal3d/src/cal3d/coreanimatedmorph.cpp
cal3d/cal3d/src/cal3d/coreanimation.cpp
cal3d/cal3d/src/cal3d/corebone.cpp
cal3d/cal3d/src/cal3d/corekeyframe.cpp
cal3d/cal3d/src/cal3d/corematerial.cpp
cal3d/cal3d/src/cal3d/coremesh.cpp
cal3d/cal3d/src/cal3d/coremodel.cpp
cal3d/cal3d/src/cal3d/coremorphkeyframe.cpp
cal3d/cal3d/src/cal3d/coremorphtrack.cpp
cal3d/cal3d/src/cal3d/coreskeleton.cpp
cal3d/cal3d/src/cal3d/coresubmesh.cpp
cal3d/cal3d/src/cal3d/coresubmorphtarget.cpp
cal3d/cal3d/src/cal3d/coretrack.cpp
cal3d/cal3d/src/cal3d/dualquaternion.cpp
cal3d/cal3d/src/cal3d/error.cpp
cal3d/cal3d/src/cal3d/global.cpp
cal3d/cal3d/src/cal3d/hardwaremodel.cpp
cal3d/cal3d/src/cal3d/loader.cpp
cal3d/cal3d/src/cal3d/matrix.cpp
cal3d/cal3d/src/cal3d/mesh.cpp
cal3d/cal3d/src/cal3d/mixer.cpp
cal3d/cal3d/src/cal3d/model.cpp
cal3d/cal3d/src/cal3d/morphtargetmixer.cpp
cal3d/cal3d/src/cal3d/physique.cpp
cal3d/cal3d/src/cal3d/physiquedualquaternion.cpp
cal3d/cal3d/src/cal3d/platform.cpp
cal3d/cal3d/src/cal3d/quaternion.cpp
cal3d/cal3d/src/cal3d/renderer.cpp
cal3d/cal3d/src/cal3d/saver.cpp
cal3d/cal3d/src/cal3d/skeleton.cpp
cal3d/cal3d/src/cal3d/springsystem.cpp
cal3d/cal3d/src/cal3d/streamsource.cpp
cal3d/cal3d/src/cal3d/submesh.cpp
cal3d/cal3d/src/cal3d/tinybind.cpp
cal3d/cal3d/src/cal3d/tinyxml.cpp
cal3d/cal3d/src/cal3d/tinyxmlerror.cpp
cal3d/cal3d/src/cal3d/tinyxmlparser.cpp
cal3d/cal3d/src/cal3d/vector.cpp
cal3d/cal3d/src/cal3d/xmlformat.cpp
)

file(GLOB_RECURSE HEADER_FILES *.h)

# include common
include(../cmake_globals.txt)

# actual compilation target
CREATE_MODULE(${module_id})

# include common
include(../cmake_suffix.txt)
