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



#include "vsx_widget_controller_editor.h"
#include "widgets/vsx_widget_panel.h"
#include "widgets/vsx_widget_editor.h"
#include "widgets/vsx_widget_button.h"
#include <gl_helper.h>


vsx_widget_controller_editor::vsx_widget_controller_editor()
{
  init();
  allow_resize_x = true;
  allow_resize_y = true;
	panel = add(new vsx_widget_split_panel,"p");
	vsx_widget* b_panel = ((vsx_widget_split_panel*)panel)->one->add(new vsx_widget_split_panel,"p2");
	editor = ((vsx_widget_split_panel*)b_panel)->two->add(new vsx_widget_editor,"e1");
  ((vsx_widget_editor*)editor)->editor->draw_line_numbers = true;
	((vsx_widget_split_panel*)b_panel)->two->size_min.y = 0.02;
	((vsx_widget_split_panel*)b_panel)->one->size_min.y = 0.02;

  e_log = (vsx_widget*)((vsx_widget_split_panel*)b_panel)->one->add(new vsx_widget_editor,"e2");
	((vsx_widget_base_edit*)((vsx_widget_editor*)e_log)->editor)->editing_enabled = false;
	((vsx_widget_split_panel*)panel)->split_pos = 0.9;
	((vsx_widget_split_panel*)panel)->splitter_size = 0;

  button_save = ((vsx_widget_split_panel*)panel)->two->add(new vsx_widget_button,"b2");
  button_close = ((vsx_widget_split_panel*)panel)->two->add(new vsx_widget_button,"b1");

  size.x = 0.11;
	size.y = 0.11;
	target_size = size;
	bgcolor.a = 0.0f;
  visible = 1;
  constrained_x = false;
  constrained_y = false;
  scroll_x = 0;
  scroll_y = 0;
  return_command = "";

  vsx_widget::init_children();

  ((vsx_widget_button*)button_close)->border = 0.0005;
  button_close->set_pos(vsx_vector3<>(-0.012f,0.0f));
  button_close->set_size(vsx_vector3<>(0.010f,0.005f));
  button_close->title = "close";
  button_close->commands.adds(4,"close","close","b1");

  ((vsx_widget_button*)button_save)->border = 0.0005f;
  button_save->set_size(vsx_vector3<>(0.010f,0.005f));
  button_save->set_pos(vsx_vector3<>(0.012f,0.0f));
  button_save->title = "save";
  button_save->commands.adds(4,"save","save","b2");

  return_command = "editor_save";

  size_min.x = 0.06;
  size_min.y = 0.035;

  pos.x -= size.x*0.7;
  target_pos = pos;

  longest_line = 0;
  support_interpolation = true;
}

void vsx_widget_controller_editor::i_draw()
{
  vsx_vector3<> parentpos;
  parentpos = parent->get_pos_p();
  float sx05 = size.x*0.5;
  float sy05 = size.y*0.5;
  if (parent->render_type == render_type) {
    vsx_widget_controller_base::i_draw();
  } else {
    parentpos.x = parentpos.y = 0.0f;
  }

  vsx_widget_skin::get_instance()->set_color_gl(1);
  draw_box_c(pos+parentpos,sx05,sy05);
}

void vsx_widget_controller_editor::init()
{
  coord_type = VSX_WIDGET_COORD_CENTER;
  if (init_run)
    return;
  vsx_widget::init();
  init_run = true;
}


void vsx_widget_controller_editor::command_process_back_queue(vsx_command_s *t)
{
  if (t->cmd == "close") {
    _delete();
  } else
  if (t->cmd == "save") {
    save();
  } else
  if (t->cmd == "pg64_ok") {
    // pg64_ok [comp] [param] [val] [id]
    load_text( vsx_string_helper::base64_decode(t->parts[3]));
    a_focus = this;
    k_focus = this;
  }
}

void vsx_widget_controller_editor::save()
{
  command_q_b.add_raw(return_command + " " + vsx_string_helper::base64_encode(((vsx_widget_editor*)editor)->get_string()) + " foo "+ target_param);
  return_component->vsx_command_queue_b(this);
}

bool vsx_widget_controller_editor::load_text(vsx_string<>new_text)
{
  ((vsx_widget_editor*)editor)->set_string(new_text);
  return true;
}


