#ifndef VSX_LOGO_INTRO_H
#define VSX_LOGO_INTRO_H
#include <vsx_platform.h>
#include "vsx_timer.h"
#include "vsx_texture.h"
#include "vsx_math_3d.h"
//#ifdef FOO
class vsx_logo_intro {
  vsx_texture* luna;
  vsx_texture* luna_bkg;
  
  float logo_time;
  vsx_vector logo_pos, logo_size;
  float logo_rot1, logo_rot2, logo_rot3;
  vsx_timer timer;


public:
  int window_width, window_height;
  void draw(bool always = false);  
  vsx_logo_intro();
  void reinit();
}; 
//#endif


#endif
