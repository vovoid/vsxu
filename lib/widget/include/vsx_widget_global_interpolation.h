/**
* Project: VSXu: Realtime modular visual programming language, music/audio visualizer.
*
* This file is part of Vovoid VSXu.
*
* @author Jonatan Wallmander, Robert Wenzel, Vovoid Media Technologies AB Copyright (C) 2003-2013
* @see The GNU Public License (GPL)
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
* or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#include <vsx_widget_dllimport.h>

class vsx_widget_global_interpolation
{
  float interpolation_speed;

public:

  vsx_widget_global_interpolation()
    :
      interpolation_speed( 10.0f )
  {}

  float get()
  {
    return interpolation_speed;
  }

  void set(float f)
  {
    interpolation_speed = f;
  }

private:
  WIDGET_DLLIMPORT static vsx_widget_global_interpolation instance;
public:

  static vsx_widget_global_interpolation* get_instance()
  {
    return &instance;
  }
};
