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

#include <math/vector/vsx_vector3_helper.h>
#include "vsx_widget.h"
#include "vsx_widget_dllimport.h"
#include <gl_helper.h>

class WIDGET_DLLIMPORT vsx_widget_popup_menu : public vsx_widget
{
  bool oversub = false;

public:

  vsx_command_list menu_items;
  vsx_command_s* current_command_over = 0x0;

  std::function<void(vsx_command_s&)> on_selection = [](vsx_command_s& command){};

  int id_over = 0;
  int over = 0;

  float row_size = 0.02f;

  vsx_widget_popup_menu()
    : menu_items(false)
  {
    target_size.x = size.x = 0.4f;
  }

  void init()
  {
    req(!init_run);

    widget_type = VSX_WIDGET_TYPE_2D_MENU_POPUP;
    coord_type = VSX_WIDGET_COORD_CORNER;
    coord_related_parent = false;
    render_type = render_2d;

    topmost = true;
    visible = 0;

    commands.reset();
    vsx_command_s* t;
    while ( (t = commands.pop()) )
      add_commands(t);
    init_run = true;
  }

  void init_extra_commands()
  {
    commands.reset();
    vsx_command_s* t;
    while ( (t = commands.pop()) )
      add_commands(t);
  }

  void show()
  {
    req(parent);
    parent->front(this);
    visible = 2;
  }

  void row_size_by_font_size(float font_size)
  {
    row_size = font_size;
  }

  void command_process_back_queue(vsx_command_s *t)
  {
    visible = 0;
    vsx_widget::command_process_back_queue(t);
  }

  void add_commands(vsx_command_s* command)
  {
    // split the title
    vsx_string<> title = command->title;
    vsx_string<> deli = ";";
    vsx_nw_vector<vsx_string<> > command_path_parts;
    vsx_string_helper::explode(title,deli,command_path_parts);

    // deepest level reached
    if (command_path_parts.size() == 1)
      return (void)menu_items.add(command);

    // multiple levels left
    vsx_widget_popup_menu* sub_menu = 0;
    if (l_list.find(command_path_parts[0]) == l_list.end())
    {
      sub_menu = dynamic_cast<vsx_widget_popup_menu*>( add(new vsx_widget_popup_menu, command_path_parts[0]) );
      sub_menu->pos.x = 0;
      sub_menu->pos.y = pos.y;
      sub_menu->init();
      menu_items.adds(VSX_COMMAND_MENU, command_path_parts[0], "", "");
    }
    else
      sub_menu = dynamic_cast<vsx_widget_popup_menu*>( l_list[command_path_parts[0]] );

    command->title = vsx_string_helper::implode(command_path_parts, deli, 1);
    ((vsx_widget_popup_menu*)sub_menu)->add_commands(command);
  }


  void event_mouse_down(vsx_widget_distance distance,vsx_widget_coords coords,int button)
  {
    VSX_UNUSED(distance);
    VSX_UNUSED(button);
    req(over);
    req(!oversub);
    req(current_command_over);

    on_selection(*current_command_over);

    // add a number of commands
    vsx_command_s* command = command_q_b.addc(current_command_over, true);
    if (command->cmd_data == "$mpos")
    {
      command->cmd_data = vsx_vector3_helper::to_string
      (
        coords.world_global-parent->get_pos_p(),
        2
      );

      if (command->parts.size() > 2)
        command->parts[2] = command->cmd_data;
    }
    parent->vsx_command_queue_b(this);
    visible = 0;
  }

  void event_mouse_move_passive(vsx_widget_distance distance,vsx_widget_coords coords)
  {
    VSX_UNUSED(coords);
    over = (int)menu_items.count()-((int)((distance.corner.y)/row_size));
  }

  void i_draw()
  {
    target_size.y = (float)(menu_items.count())*row_size;

    req(visible > 0.0f);

    if (a_focus->widget_type != VSX_WIDGET_TYPE_2D_MENU_POPUP && !FLOAT_EQUALS(visible, 2.0f) )
    {
      visible = 0.0f;
      return;
    }

    // in case of another menu being open before us
    if (FLOAT_EQUALS(visible, 2.0f) && a_focus->widget_type == VSX_WIDGET_TYPE_2D_MENU_POPUP)
      if (a_focus != this)
        if (a_focus != parent)
          a_focus->visible = 0;

    if (FLOAT_EQUALS(visible, 2.0f))
    {
      hide_children();
      over = 0;
      oversub = false;
      target_pos.y -= target_size.y;
      pos.y = target_pos.y;
    }

    visible = 1.0f;
    a_focus = this;
    if (parent->widget_type != VSX_WIDGET_TYPE_2D_MENU_POPUP)
      if (pos.x+size.x > 1)
        pos.x = 1-size.x;

    float sx = target_pos.x;

    font.color = vsx_color<>(1,1,1,1);

    vsx_color<> background_color = vsx_widget_skin::get_instance()->get_color(0);
    glColor4f(background_color.r, background_color.g, background_color.b, 0.9f);
//    vsx_widget_skin::get_instance()->set_color_gl(0);

    if (parent->widget_type == VSX_WIDGET_TYPE_2D_MENU_POPUP) {
      if (((vsx_widget_popup_menu*)parent)->over && ((vsx_widget_popup_menu*)parent)->over != id_over)
        visible = 0;
      else
        draw_box(vsx_vector3<>((sx-0.001f)*screen_aspect,target_pos.y+0.001f+target_size.y),(size.x+0.002f)*screen_aspect,-((float)menu_items.count())*row_size-0.002f);
    }
    else
    {
      draw_box(vsx_vector3<>((sx-0.001f)*screen_aspect,target_pos.y+0.001f+target_size.y),(size.x+0.002f)*screen_aspect,-((float)menu_items.count())*row_size-0.002f);
      font.print(vsx_vector3<>(sx*screen_aspect,target_pos.y+target_size.y), parent->title, row_size);
    }

    float y = target_pos.y+target_size.y;
    vsx_command_s *t;

    menu_items.reset();
    int c = 1;
    while ( (t = menu_items.get()) )
    {
      if (c == over && over != 0)
      {
        current_command_over = t;
        vsx_widget_skin::get_instance()->set_color_gl(4);

        if (t->cmd == "" && l_list.find(t->title) != l_list.end())
        {
          vsx_widget_popup_menu* sub = ((vsx_widget_popup_menu*)l_list[t->title]);
          if (!(sub->visible > 0.0f))
          {
            sub->visible = 2;
            sub->target_pos.x = target_pos.x+(1/screen_aspect)*row_size*(float)t->title.size()*0.33f;
            sub->target_pos.y = y;
            sub->size = size;
            sub->id_over = over;
          }
          oversub = true;
        } else {
          oversub = false;
        }
      }
      else
        vsx_widget_skin::get_instance()->set_color_gl(1);

      draw_box(vsx_vector3<>((sx)*screen_aspect,y),size.x*screen_aspect,-row_size);
      font.print(vsx_vector3<>((sx+0.003f)*screen_aspect,y-row_size),t->title,row_size*0.8f);
      y -= row_size;
      ++c;
    }
    draw_children_2d();
  }

  void on_delete()
  {
    menu_items.clear_delete();
  }

};
