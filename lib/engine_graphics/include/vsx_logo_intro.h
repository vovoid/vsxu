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
#include <engine_graphics_dllimport.h>
#include <time/vsx_timer.h>
#include <texture/vsx_texture.h>


class vsx_logo_intro {
  std::unique_ptr<vsx_texture<>> luna;
  std::unique_ptr<vsx_texture<>> luna_bkg;
  
  float logo_time;
  vsx_vector3<> logo_pos, logo_size;
  float logo_rot1, logo_rot2, logo_rot3;
  vsx_timer timer;
  bool destroy_textures;

public:
  int window_width, window_height;
  VSX_ENGINE_GRAPHICS_DLLIMPORT void draw(bool always = false,bool draw_background = true, bool draw_black_overlay = true);
  VSX_ENGINE_GRAPHICS_DLLIMPORT vsx_logo_intro();
  void set_destroy_textures(bool new_value)
  {
    destroy_textures = new_value;
  }

  VSX_ENGINE_GRAPHICS_DLLIMPORT void reinit();
}; 



#endif
