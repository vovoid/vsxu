#ifndef __vsx_mouse__
#define __vsx_mouse__
#ifdef _WIN32
#include <windows.h>
#else
#include <X11/Xlib.h>
#include <X11/cursorfont.h>
#endif
#include "vsx_math_3d.h"


#define MOUSE_CURSOR_ARROW 0
#define MOUSE_CURSOR_HAND 1
#define MOUSE_CURSOR_IBEAM 2
#define MOUSE_CURSOR_SIZE 3
#define MOUSE_CURSOR_NS 4
#define MOUSE_CURSOR_WE 5 
#define MOUSE_CURSOR_NESW 6
#define MOUSE_CURSOR_NWSE 7

class vsx_mouse
{
  #ifndef _WIN32
  Display *dpy;
  Window   win;
  Cursor pointer;
  #endif
  public:
  int cursor;
  bool visible;
  vsx_vector position;
  vsx_vector get_cursor_pos() {return position;}
  void set_cursor_implement();
  void set_cursor(int id);
  void set_cursor_pos(float x, float y);
  void show_cursor();
  void hide_cursor();
  vsx_mouse();
};
#endif

