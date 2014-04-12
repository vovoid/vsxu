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


#include "vsx_widget_2d_label.h"

void vsx_widget_2d_label::init()
{
  halign = a_center;
  font_size = size.y = 0.014;

}

void vsx_widget_2d_label::draw_2d()
{
  if (!visible) return;
  glColor3f(1,1,1);
  vsx_vector<> p = parent->get_pos_p()+pos;
  p.y -= font_size*0.5f;
  switch ((align)halign) {
    case a_left:
      font.print(p, title,font_size);
    break;
    case a_center:
      font.print_center(p, title,font_size);
    break;
    case a_right:
      font.print_right(p, title,font_size);
    break;
  }
}

