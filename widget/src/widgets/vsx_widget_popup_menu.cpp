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

#include <vsxfst.h>
#include "vsx_widget_popup_menu.h"
#include <gl_helper.h>
#include <vector/vsx_vector3_helper.h>

void vsx_widget_popup_menu::i_draw()
{
  render_type =   render_2d;

  target_size.y = (float)(menu_items.count())*row_size;

  if (visible >= 1) {
    if (a_focus->widget_type != VSX_WIDGET_TYPE_2D_MENU_POPUP && visible != 2) {
      visible = 0;
      return;
    } else {
      if (visible == 2 && a_focus->widget_type == VSX_WIDGET_TYPE_2D_MENU_POPUP) {
        // in case of another menu being open before us
        if (a_focus != this)
        if (a_focus != parent) {
          a_focus->visible = 0;
        }
      }
      if (visible == 2) {
        hide_children();
        over = 0;
        oversub = false;
        target_pos.y -= target_size.y;
        pos.y = target_pos.y;
      }
      visible = 1;
      a_focus = this;
      if (parent->widget_type != VSX_WIDGET_TYPE_2D_MENU_POPUP) {
        if (pos.x+size.x > 1) pos.x = 1-size.x;
      }
    }
    float sx = target_pos.x;

    font.color = vsx_color<>(1,1,1,1);



    vsx_widget_skin::get_instance()->set_color_gl(0);

    if (parent->widget_type == VSX_WIDGET_TYPE_2D_MENU_POPUP) {
      if (((vsx_widget_popup_menu*)parent)->over && ((vsx_widget_popup_menu*)parent)->over != id_over)
        visible = 0;
      else
      {
        draw_box(vsx_vector3<>((sx-0.001)*screen_aspect,target_pos.y+0.001+target_size.y),(size.x+0.002)*screen_aspect,-((float)menu_items.count())*row_size-0.002);
      }
    }
    else
    {
      draw_box(vsx_vector3<>((sx-0.001)*screen_aspect,target_pos.y+0.001+target_size.y),(size.x+0.002)*screen_aspect,-((float)menu_items.count())*row_size-0.002);
      font.print(vsx_vector3<>(sx*screen_aspect,target_pos.y+target_size.y),parent->title,row_size);
    }


    double y = target_pos.y+target_size.y;
    vsx_command_s *t;

    menu_items.reset();
    int c = 1;
    while ( (t = menu_items.get()) ) {
      if (c == over && over != 0)
      {
        current_command_over = t;
        vsx_widget_skin::get_instance()->set_color_gl(4);

        if (t->cmd == "" && l_list.find(t->title) != l_list.end())
        {
          vsx_widget_popup_menu* sub = ((vsx_widget_popup_menu*)l_list[t->title]);
          if (!sub->visible) {
            sub->visible = 2;
            sub->target_pos.x = target_pos.x+(1/screen_aspect)*row_size*(float)t->title.size()*0.33;
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
      font.print(vsx_vector3<>((sx+0.003)*screen_aspect,y-row_size),t->title,row_size*0.8);
      y-=row_size;
      ++c;
    }
    draw_children_2d();
  }
}

void vsx_widget_popup_menu::add_commands(vsx_command_s *command) {
  // split the title
  vsx_string<>title = command->title;
  vsx_string<>deli = ";";
  std::vector <vsx_string<> > add_c;
  explode(title,deli,add_c);
  if (add_c.size() > 1) {
    vsx_widget* t = 0;
    if (l_list.find(add_c[0]) == l_list.end()) {
      t = add(new vsx_widget_popup_menu, add_c[0]);
      t->pos.x = 0;
      t->pos.y = pos.y;
      t->init();
      menu_items.adds(VSX_COMMAND_MENU, add_c[0],"","");
    } else t = l_list[add_c[0]];
    add_c.erase(add_c.begin());
    command->title = implode(add_c,deli);
    ((vsx_widget_popup_menu*)t)->add_commands(command);
  } else {
    menu_items.add(command);
  }
}

void vsx_widget_popup_menu::init() {
  if (init_run) return;
  widget_type = VSX_WIDGET_TYPE_2D_MENU_POPUP;
  coord_type = VSX_WIDGET_COORD_CORNER;
  coord_related_parent = false;

  topmost = true;
  row_size = 0.02;
  visible = 0;
  over = 0;
  oversub = false;


  commands.reset();
  vsx_command_s* t;
  while ( (t = commands.get()) ) {
    add_commands(t);
    //

  }
  init_run = true;
}

void vsx_widget_popup_menu::event_mouse_move_passive(vsx_widget_distance distance,vsx_widget_coords coords)
{
  VSX_UNUSED(coords);
  //printf("menu type: %d\n",render_type);
  //printf("menu passive %d %f %f \n",menu_items.count(),target_pos.y,distance.corner.y);

  //!glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
    over = menu_items.count()-((int)((distance.corner.y)/row_size));
}

void vsx_widget_popup_menu::command_process_back_queue(vsx_command_s *t) {
  visible = 0;
  vsx_widget::command_process_back_queue(t);
}

void vsx_widget_popup_menu::event_mouse_down(vsx_widget_distance distance,vsx_widget_coords coords,int button)
{
  VSX_UNUSED(distance);
  VSX_UNUSED(button);
  if (over) {
    if (!oversub) {
      // add a number of commands
      vsx_command_s* t = command_q_b.addc(current_command_over, true);
      if (t-> cmd_data == "$mpos")
      {
        t->cmd_data = vsx_vector3_helper::to_string
        (
          coords.world_global-parent->get_pos_p(),
          2
        );

        if (t->parts.size() > 2) t->parts[2] = t->cmd_data;
      }
      parent->vsx_command_queue_b(this);
      visible = 0;
    }
  }
}

void vsx_widget_popup_menu::on_delete()
{
  menu_items.clear_delete();
}
