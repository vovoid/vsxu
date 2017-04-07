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

#include <vsx_gl_global.h>
#include <widgets/vsx_widget_2d_pager.h>

vsx_widget_2d_pager::vsx_widget_2d_pager()
{
  type = 0;
  max_page = 0;
  cur_page = 0;
}

void vsx_widget_2d_pager::init()
{
  set_size(vsx_vector3<>(0.06f, 0.1f));
  pos.x = 0.0f;
  pos.y = 0.0f;
  render_type = render_2d;
  coord_type = VSX_WIDGET_COORD_CENTER;
}


int vsx_widget_2d_pager::get_cur_page()
{
  return cur_page;
}

void vsx_widget_2d_pager::set_cur_page(int n)
{
  cur_page = n;
}


int vsx_widget_2d_pager::get_max_page()
{
  return max_page;
}

void vsx_widget_2d_pager::set_max_page(int n)
{
  max_page = n;
}


void vsx_widget_2d_pager::event_mouse_down(vsx_widget_distance distance,vsx_widget_coords coords,int button)
{
  VSX_UNUSED(coords);
  VSX_UNUSED(button);

  distance.center.x > 0.0f ? increase() : decrease();
}

void vsx_widget_2d_pager::i_draw()
{
  req(visible > 0.0f);
  if (max_page == 0) cur_page = -1;
  font.print_center(get_pos_p(), "<< "+vsx_string_helper::i2s(cur_page+1)+" / "+vsx_string_helper::i2s(max_page)+" >>", size.y*0.6f);
}

void vsx_widget_2d_pager::increase()
{
  ++cur_page;
  if (cur_page >= max_page) cur_page = 0;
}

void vsx_widget_2d_pager::decrease()
{
  if (max_page && cur_page) --cur_page; else cur_page = max_page-1;
}

