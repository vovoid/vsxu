#ifndef VSX_WIDGET_COORDS_H
#define VSX_WIDGET_COORDS_H


// coordinate-system types
#define VSX_WIDGET_COORD_CENTER 1
#define VSX_WIDGET_COORD_CORNER 2

#include <math/vector/vsx_vector3.h>

class vsx_widget_coords
{
  static double modelMatrix[16];
  static double projMatrix[16];
  static int    viewport[4];
public:
  vsx_vector3<> world_local; // in local widget coordinates
  vsx_vector3<> world_global; // untouched global coordinates
  vsx_vector3<> screen_local; // in local widget coordinates
  vsx_vector3<> screen_global; // untouched global coordinates
  void init(float x, float y);
};

#endif
