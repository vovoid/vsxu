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

#include <vsx_widget.h>
#include <gl_helper.h>

enum align{
  a_left,
  a_center,
  a_right
};

class WIDGET_DLLIMPORT vsx_widget_label
  :
    public vsx_widget
{
  bool outside = false;

public:

  std::function<void()> on_click = [](){};

  bool inside_xyd(vsx_vector3<> world, vsx_vector3<> screen)
  {
    VSX_UNUSED(world);
    VSX_UNUSED(screen);

    return false;
  }

  align halign;

  void init()
  {
    halign = a_center;
    font_size = size.y = 0.014f;
    set_render_type(vsx_widget_render_type::render_2d);
  }

  void event_mouse_down(vsx_widget_distance distance,vsx_widget_coords coords,int button)
  {
    VSX_UNUSED(distance);
    VSX_UNUSED(coords);
    VSX_UNUSED(button);

    outside = false;
    m_focus = this;
  }

  void event_mouse_up(vsx_widget_distance distance,vsx_widget_coords coords,int button)
  {
    VSX_UNUSED(distance);
    VSX_UNUSED(coords);
    VSX_UNUSED(button);
    req(!outside);

    on_click();
  }

  void event_mouse_move(vsx_widget_distance distance,vsx_widget_coords coords)
  {
    VSX_UNUSED(coords);
    outside =
      !(
        (distance.corner.x > 0)
        &&
        (distance.corner.x < target_size.x)
        &&
        (distance.corner.y > 0)
        &&
        (distance.corner.y < target_size.y)
      );
  }

  void draw_debug(vsx_vector3f position)
  {
    draw_box_gradient(
          position - vsx_vector3f(size.x * 0.5f, size.y * 0.5f),
          size.x, size.y,
          vsx_widget_skin::get_instance()->get_color(3), vsx_widget_skin::get_instance()->get_color(3),
          vsx_widget_skin::get_instance()->get_color(4), vsx_widget_skin::get_instance()->get_color(4)
    );
  }

  void i_draw()
  {
    req(visible);

    vsx_vector3<> position = parent->get_pos_p() + pos;

//    draw_debug(position);

    glColor3f(1,1,1);
    position.y -= font_size*0.5f;
    switch ((align)halign) {
      case a_left:
        position.x -= size.x * 0.5f;
        font.print(position, title,font_size);
      break;
      case a_center:
        font.print_center(position, title,font_size);
      break;
      case a_right:
        font.print_right(position, title,font_size);
      break;
    }
  }



};
