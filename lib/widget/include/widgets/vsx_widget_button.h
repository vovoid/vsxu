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

#include <functional>
#include <vsx_widget.h>


class WIDGET_DLLIMPORT vsx_widget_button
  : public vsx_widget
{
  bool outside = false;

public:

  float border = 0.0f;

  std::function<void()> on_click = [](){};


  vsx_widget_button()
  {
    set_render_type( vsx_widget_render_type::render_2d );
  }

  void init()
  {
    coord_type = VSX_WIDGET_COORD_CENTER;
    if (render_type == render_2d)
    {
      border = 0.0023f;
      size.x = 0.06f;
      size.y = 0.03f;
    } else {
      border = 0.0009f;
      size.x = 0.03f;
      size.y = 0.015f;
    }
    target_size = size;
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

    // 1. call event
    on_click();

    // 2. send command
    req(commands.count());
    commands.reset();
    command_q_b.addc(commands.get_cur());
    parent->vsx_command_queue_b(this);
  }

  void event_mouse_move(vsx_widget_distance distance,vsx_widget_coords coords)
  {
    VSX_UNUSED(coords);
    outside = !((distance.corner.x > 0) && (distance.corner.x < target_size.x) && (distance.corner.y > 0) && (distance.corner.y < target_size.y));
  }

  void i_draw()
  {
    req(visible > 0.0f);

    vsx_vector3<> i_pos = get_pos_p();

    if (coord_type == VSX_WIDGET_COORD_CENTER)
    {
      i_pos.x -= size.x*0.5f;
      i_pos.y += size.y*0.5f;
    }

    float font_size_smaller = 1.0f;

    if ((m_focus == this) && !outside)
    {
      font_size_smaller = 0.75f;
      vsx_widget_skin::get_instance()->set_color_gl(0);
    }
    else
    {
      vsx_widget_skin::get_instance()->set_color_gl(6);
    }

    glBegin(GL_QUADS);
      glVertex3f((i_pos.x+border)*screen_aspect           , i_pos.y-size.y+(float)border, i_pos.z);
      glVertex3f((i_pos.x+border)*screen_aspect           , i_pos.y-(float)border, i_pos.z);
      glVertex3f((i_pos.x+size.x-border)*screen_aspect    , i_pos.y-(float)border, i_pos.z);
      glVertex3f((i_pos.x+size.x-border)*screen_aspect    , i_pos.y-size.y+(float)border, i_pos.z);
    glEnd();



    // white border
    if ((m_focus == this) && (!outside))
    {
      vsx_widget_skin::get_instance()->set_color_gl(1);
    }
    else
    {
      vsx_widget_skin::get_instance()->set_color_gl(4);
    }

    glBegin(GL_QUADS);
      // left
      glVertex3f(i_pos.x, i_pos.y-border, i_pos.z);
      glVertex3f((i_pos.x+border), i_pos.y-border, i_pos.z);
      glVertex3f((i_pos.x+border), i_pos.y-size.y+border, i_pos.z);
      glVertex3f(i_pos.x,i_pos.y-size.y+border, i_pos.z);

      // right
      glVertex3f((i_pos.x+size.x-border), i_pos.y-border, i_pos.z);
      glVertex3f((i_pos.x+size.x), i_pos.y-border, pos.z);
      glVertex3f((i_pos.x+size.x), i_pos.y-size.y+border, i_pos.z);
      glVertex3f((i_pos.x+size.x-border),i_pos.y-size.y+border, i_pos.z);

      glVertex3f(i_pos.x, i_pos.y, i_pos.z);
      glVertex3f((i_pos.x+size.x), i_pos.y, i_pos.z);
      glVertex3f((i_pos.x+size.x), i_pos.y-border, i_pos.z);
      glVertex3f(i_pos.x,i_pos.y-border, i_pos.z);

      glVertex3f(i_pos.x, i_pos.y-size.y+border, i_pos.z);
      glVertex3f((i_pos.x+size.x), i_pos.y-size.y+border, i_pos.z);
      glVertex3f((i_pos.x+size.x), i_pos.y-size.y, i_pos.z);
      glVertex3f(i_pos.x,i_pos.y-size.y, i_pos.z);
    glEnd();

    float dd = 0.0f;
    if ((m_focus == this) && (!outside))
      dd = size.y*0.05f;

    font.color = vsx_color<>(1,1,1,1);

    font.print_center
    (
      vsx_vector3<>( (i_pos.x + size.x * 0.5f + dd),  i_pos.y-size.y+size.y * 0.25f),
      title,
      size.y * 0.4f * font_size_smaller
    );
  }

  void on_delete()
  {
    commands.clear_delete();
  }
};
