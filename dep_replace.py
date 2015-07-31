#!/usr/bin/env python

import sys

#GENERATED_DEP_START
#GENERATED_DEP_STOP

def section_replace(buf, new, start = '\n#GENERATED_DEP_START', stop  = '\n#GENERATED_DEP_STOP'):
    try:
        head, more = buf.split(start,1)
        old, tail = more.split(stop, 1)
        return head + start + new + stop + tail
    except:
        return None

new = """
INCLUDE(FindPkgConfig)
PKG_SEARCH_MODULE(FREETYPE REQUIRED freetype2)
PKG_SEARCH_MODULE(PNG REQUIRED  libpng)
PKG_SEARCH_MODULE(ZLIB REQUIRED zlib)
PKG_SEARCH_MODULE(JPEG REQUIRED libjpeg)
PKG_SEARCH_MODULE(GLEW REQUIRED glew)
PKG_SEARCH_MODULE(GLFW REQUIRED libglfw)

include_directories(
  ${FREETYPE_INCLUDE_DIRS}
  ${PNG_INCLUDE_DIRS}
  ${ZLIB_INCLUDE_DIRS}
  ${JPEG_INCLUDE_DIRS}
  ${GLEW_INCLUDE_DIRS}
  ${GLFW_INCLUDE_DIRS}
)

link_directories(
  ${FREETYPE_LIBRARY_DIRS}
  ${PNG_LIBRARY_DIRS}
  ${ZLIB_LIBRARY_DIRS}
  ${JPEG_LIBRARY_DIRS}
  ${GLEW_LIBRARY_DIRS}
  ${GLFW_LIBRARY_DIRS}
)"""

for fn in sys.argv[1:]:
    f = open(fn, "r")
    c = section_replace( f.read(), new )
    f.close()
    if c:
        f = open(fn, "w")
        f.write(c)
        f.close()

