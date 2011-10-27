# Try to find the PulseAudio library
#
# Once done this will define:
#
#  PULSEAUDIO_FOUND - system has the PulseAudio library
#  PULSEAUDIO_INCLUDE_PATH - the PulseAudio include directory
#  PULSEAUDIO_LIBRARY - the libraries needed to use PulseAudio
#  PULSEAUDIO_MAINLOOP_LIBRARY - the libraries needed to use PulsAudio Mailoop
#
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

if (NOT PULSEAUDIO_MINIMUM_VERSION)
  set(PULSEAUDIO_MINIMUM_VERSION "0.9.9")
endif (NOT PULSEAUDIO_MINIMUM_VERSION)

if (PULSEAUDIO_INCLUDE_PATH AND PULSEAUDIO_LIBRARY AND PULSEAUDIO_MAINLOOP_LIBRARY)
   # Already in cache, be silent
   set(PULSEAUDIO_FIND_QUIETLY TRUE)
endif (PULSEAUDIO_INCLUDE_PATH AND PULSEAUDIO_LIBRARY AND PULSEAUDIO_MAINLOOP_LIBRARY)

if (NOT WIN32)
   include(FindPkgConfig)
   pkg_check_modules(PC_PULSEAUDIO libpulse>=${PULSEAUDIO_MINIMUM_VERSION})
   pkg_check_modules(PC_PULSEAUDIO_MAINLOOP libpulse-mainloop-glib)
endif (NOT WIN32)

FIND_PATH(PULSEAUDIO_INCLUDE_PATH pulse/pulseaudio.h
   HINTS
   ${PC_PULSEAUDIO_INCLUDEDIR}
   ${PC_PULSEAUDIO_INCLUDE_DIRS}
   )

FIND_LIBRARY(PULSEAUDIO_LIBRARY NAMES pulse libpulse
   HINTS
   ${PC_PULSEAUDIO_LIBDIR}
   ${PC_PULSEAUDIO_LIBRARY_DIRS}
   )

FIND_LIBRARY(PULSEAUDIO_MAINLOOP_LIBRARY NAMES pulse-mainloop pulse-mainloop-glib libpulse-mainloop-glib
   HINTS
   ${PC_PULSEAUDIO_LIBDIR}
   ${PC_PULSEAUDIO_LIBRARY_DIRS}
   )

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(PULSEAUDIO DEFAULT_MSG PULSEAUDIO_LIBRARY PULSEAUDIO_INCLUDE_PATH)

mark_as_advanced(PULSEAUDIO_INCLUDE_PATH PULSEAUDIO_LIBRARY PULSEAUDIO_MAINLOOP_LIBRARY)