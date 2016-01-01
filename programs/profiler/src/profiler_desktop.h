/**
* Project: VSXu Profiler - Data collection and data visualizer
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

#ifndef VSX_WIDGET_DESKTOP_H
#define VSX_WIDGET_DESKTOP_H

#include <vsx_widget.h>

#include "profiler/vsx_widget_profiler.h"

class vsx_widget_desktop : public vsx_widget
{
  vsx_widget_profiler* profiler;
  vsx_texture<>* mtex;

public:
  vsx_command_list* system_command_queue;

  double xpp,ypp,zpp;
  void init();
  void reinit();
  bool key_down(signed long key, bool n_alt, bool n_ctrl, bool n_shift);
  bool key_up(signed long key, bool alt, bool ctrl, bool shift);
  void event_mouse_wheel(float y);
  void command_process_back_queue(vsx_command_s *t);
  void draw();
  void draw_2d();
  vsx_widget_desktop();
  ~vsx_widget_desktop();
};








#endif
