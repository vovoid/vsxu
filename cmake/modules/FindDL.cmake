# - find where dlopen and friends are located.
# DL_FOUND - system has dynamic linking interface
# DL_INCLUDE_DIR - where dlfcn.h is located
# DL_LIBRARY - libraries needed to use flopen

include (CheckFunctionExists)

find_path(DL_INCLUDE_DIR NAMES dlfcn.h)
find_library(DL_LIBRARY NAMES dl)

if (DL_LIBRARY)
  set (DL_FOUND)
else (DL_LIBRARY)
  #if dlopen is found without linking,
  #then dlopen is part of libc, so we dont needed
  #to link extra libraries
  check_function_exists(dlopen DL_FOUND)
  SET(DL_LIBRARY "")
endif (DL_LIBRARY)