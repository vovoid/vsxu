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

#ifndef VSX_WIDGET_CONTROLLER_CHANNEL_H
#define VSX_WIDGET_CONTROLLER_CHANNEL_H

#include "vsx_widget_controller_base.h"


class vsx_widget_controller_channel : public vsx_widget_controller_base
{
public:
  vsx_widget* amp;
  vsx_widget* offset;
  vsx_widget* slider;
  vsx_widget* label;
  vsx_widget_controller_channel();
  int inside_xy_l(vsx_vector3<> &test, vsx_vector3<> &global)
  {
    if (owned)
      return 0;
    return
      vsx_widget::inside_xy_l(test,global);
  }
  void smooth(float newval);
  void init();
  void draw();
  void command_process_back_queue(vsx_command_s *t);
};


#endif
