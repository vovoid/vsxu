# Locate the glfw library
# This module defines the following variables:
# GLFW_LIBRARY, the name of the library;
# GLFW_INCLUDE_DIR, where to find glfw include files.
# GLFW_FOUND, true if both the GLFW_LIBRARY and GLFW_INCLUDE_DIR have been found.
#
# To help locate the library and include file, you could define an environment variable called
# GLFW_ROOT which points to the root of the glfw library installation. This is pretty useful
# on a Windows platform.
#
#
# Usage example to compile an "executable" target to the glfw library:
#
# FIND_PACKAGE (glfw REQUIRED)
# INCLUDE_DIRECTORIES (${GLFW_INCLUDE_DIR})
# ADD_EXECUTABLE (executable ${EXECUTABLE_SRCS})
# TARGET_LINK_LIBRARIES (executable ${GLFW_LIBRARY})
#
# TODO:
# Allow the user to select to link to a shared library or to a static library.

#Search for the include file...
FIND_PATH(GLFW_INCLUDE_PATH GL/glfw.h DOC "Path to GLFW include directory."
  PATHS
    $ENV{DEPINSTALL}/include
    /usr/include/
    /usr/local/include/
    # By default headers are under GL subfolder
    /usr/include/GL
    /usr/local/include/GL
  NO_DEFAULT_PATH
)

FIND_LIBRARY(GLFW_LIBRARY DOC "Absolute path to GLFW library."
  NAMES glfw GLFW.lib
  PATHS
    $ENV{DEPINSTALL}/lib
    /usr/local/lib
    /usr/lib
  NO_DEFAULT_PATH
)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GLFW DEFAULT_MSG GLFW_LIBRARY GLFW_INCLUDE_PATH)
