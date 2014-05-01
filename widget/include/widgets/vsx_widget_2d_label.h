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


#ifndef VSX_WIDGET_2D_LABEL_H
#define VSX_WIDGET_2D_LABEL_H

#include "vsx_widget_base.h"

enum align{
  a_left,
  a_center,
  a_right
};

class vsx_widget_2d_label : public vsx_widget
{
public:
  bool inside_xyd(vsx_vector<> world, vsx_vector<> screen)
  {
    VSX_UNUSED(world);
    VSX_UNUSED(screen);

    return false;
  }
  align halign;
  void init();
  void draw_2d();
};


#endif // VSX_WIDGET_2D_LABEL_H
