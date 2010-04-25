#ifndef VSX_BITMAP_H
#define VSX_BITMAP_H

// everyone that wants to mess with this must make a copy of their own.
// because threads are working on it it has to be like this.
// also, if the timestamp and it's valid it's ok to read from it, otherwise
// it's not.

class vsx_bitmap {
public:
  int bpp; // bytes per pixel 3 or 4 usually
  int bformat; // pixel format, usually GL_RGB or GL_RGBA
  unsigned long size_x;
  unsigned long size_y;
  unsigned long *data;  // 32-bit
  bool valid; // while this is false, don't upload it as a texture or read it, use your old copy
  int timestamp; // increased with 1 every time it's modified so others can now and react.
  int* ref; // reference counter
  
  vsx_bitmap() {
  	ref = 0;
    timestamp = 0;
    valid = false;
    size_x = size_y = 0;
  }
};

#endif
