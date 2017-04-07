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


#pragma once

#include "vsx_widget.h"

class WIDGET_DLLIMPORT vsx_widget_2d_pager : public vsx_widget
{
  bool outside;
  //vsx_font myf;
  int side;
  int cur_page, max_page;
  int type; // 0 = 2d, 1 = 3d
  double border;
public:

  int get_cur_page();
  void set_cur_page(int n);

  int get_max_page();
  void set_max_page(int n);

  void init();
  void event_mouse_down(vsx_widget_distance distance,vsx_widget_coords coords,int button);
  void i_draw();
  void increase();
  void decrease();
  vsx_widget_2d_pager();
};
