/**
* Project: VSXu: Realtime modular visual programming engine.
*
* This file is part of Vovoid VSXu.
*
* @author Jonatan Wallmander, Robert Wenzel, Vovoid Media Technologies AB Copyright (C) 2003-2013
* @see The GNU Lesser General Public License (LGPL)
*
* VSXu Engine is free software; you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
* or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU Lesser General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/


#ifndef VSX_LOGO_INTRO_H
#define VSX_LOGO_INTRO_H
#include <vsx_platform.h>
#include "vsx_timer.h"
#include "vsx_texture.h"
#include "vsx_math_3d.h"

#if PLATFORM_FAMILY == PLATFORM_FAMILY_UNIX
#define VSX_LOGO_INTRO_DLLIMPORT
#else
  #ifdef VSX_ENG_DLL
    #define VSX_LOGO_INTRO_DLLIMPORT __declspec (dllexport) 
  #else 
    #define VSX_LOGO_INTRO_DLLIMPORT __declspec (dllimport)
  #endif
#endif


//#ifdef FOO
class vsx_logo_intro {
  vsx_texture* luna;
  vsx_texture* luna_bkg;
  
  float logo_time;
  vsx_vector logo_pos, logo_size;
  float logo_rot1, logo_rot2, logo_rot3;
  vsx_timer timer;
  bool destroy_textures;

public:
  int window_width, window_height;
  VSX_LOGO_INTRO_DLLIMPORT void draw(bool always = false,bool draw_background = true, bool draw_black_overlay = true);
  VSX_LOGO_INTRO_DLLIMPORT vsx_logo_intro();
  void set_destroy_textures(bool new_value)
  {
    destroy_textures = new_value;
  }

  VSX_LOGO_INTRO_DLLIMPORT void reinit();
}; 
//#endif


#endif
