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

#include "vsx_gl_global.h"
#include <map>
#include <list>
#include <vector>
#include <command/vsx_command.h>
#include <command/vsx_command_list.h>
#include <command/vsx_command_client_server.h>
#include <texture/vsx_texture.h>
#include "vsx_font.h"
#include "vsx_widget.h"
#include "widgets/vsx_widget_panel.h"
#include "widgets/vsx_widget_scrollbar.h"
#include "widgets/vsx_widget_base_edit.h"
#include "vsx_widget_window.h"
#include "vsx_widget_note.h"
#include <math/vector/vsx_vector3_helper.h>

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------

vsx_widget_note::vsx_widget_note() {
  vsx_widget_window::init();
  init();
  check_time = 0.0f;
  allow_resize_x = true;
  allow_resize_y = true;
  editor = add(new vsx_widget_base_edit,"e1");
  //editor->size_from_parent = true;

  size.x = 0.11;
  size.y = 0.11;
  target_size = size;
  //bgcolor.a = 0.0f;
  visible = 1;
  constrained_x = false;
  constrained_y = false;
  scroll_x = 0;
  scroll_y = 0;
  return_command = "";

  vsx_widget::init_children();

  return_command = "note_save";

  size_min.x = 0.06;
  size_min.y = 0.035;

  pos.x -= size.x*0.7;
  target_pos = pos;

  longest_line = 0;
  support_interpolation = true;
  ((vsx_widget_base_edit*)editor)->set_string("test0r");
  ((vsx_widget_base_edit*)editor)->size_from_parent = true;
  ((vsx_widget_base_edit*)editor)->enable_syntax_highlighting = false;
  title = "NOTE";
}

bool vsx_widget_note::init_from_command(vsx_command_s* c)
{
  set_render_type(render_3d);
  vsx_vector3<> np;
  np = vsx_vector3_helper::from_string<float>(c->parts[2]);
  set_pos(np);
  np = vsx_vector3_helper::from_string<float>(c->parts[3]);
  set_size(np);
  set_font_size(vsx_string_helper::s2f(c->parts[5]));
  set_border(font_size*0.15f);
  size_min.x = font_size*3.0f;
  size_min.y = font_size*4.0f;

  //new_note->target_size = new_note->size;
  load_text( vsx_string_helper::base64_decode(c->parts[4]));
  return true;
}

void vsx_widget_note::set_editor_font_size(float new_size)
{
  ((vsx_widget_base_edit*)editor)->font_size = new_size;
}

void vsx_widget_note::i_draw() {
  if (font_size != editor->font_size)
  {
    for (children_iter = children.begin(); children_iter != children.end(); children_iter++)
    {
      //if ( (*children_iter)->name == "bc1" )
      //::MessageBox(0, "note has bc1", (*children_iter)->name.c_str(), MB_OK);
    }
    font_size = editor->font_size;
    size_min.x = font_size*3.0f;
    size_min.y = font_size*4.0f;
    resize_to(target_size);
  }
  editor->set_pos(vsx_vector3<>(size.x*0.5f,size.y*0.5f-0.5f*font_size+dragborder*0.5f));
  editor->resize_to(vsx_vector3<>(size.x-dragborder*2, size.y-font_size-dragborder));

  vsx_vector3<> parentpos;
  parentpos = parent->get_pos_p();
  //float sx05 = size.x*0.5;
  //float sy05 = size.y*0.5;
  if (parent->render_type == render_type) {
    //parentpos.x = 0.0f;
    //parentpos.y = -font_size*2.0f;
    vsx_widget_window::i_draw();
  } else {
  }
  if (((vsx_widget_base_edit*)editor)->updates) {
    check_time -= vsx_widget_time::get_instance()->get_dtime();
    if (check_time < 0.01f)
    {
      save();
      ((vsx_widget_base_edit*)editor)->updates = 0;
      check_time = 1.0f;
    }
  }
//  printf("pos.x: %f\n",parentpos.x);
//  parentpos.x = 0;
//  parentpos.x = 0;
//  parentpos.z = 0;
  //glColor4f(skin_color[1].r,skin_color[1].g,skin_color[1].b,1);
  //draw_box(pos+parentpos,sx05,sy05);
}

void vsx_widget_note::init() {
  coord_type = VSX_WIDGET_COORD_CORNER;
  if (init_run) return;
  //init_children();
  vsx_widget::init();
  init_run = true;
}


void vsx_widget_note::command_process_back_queue(vsx_command_s *t) {
  if (t->cmd == "close") {
    command_q_b.add_raw("note_delete "+name);
    parent->vsx_command_queue_b(this);
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

void vsx_widget_note::save() {
  command_q_b.add_raw("note_update "+name+" "+ vsx_vector3_helper::to_string(target_pos) +" " + vsx_vector3_helper::to_string(target_size) +" "+vsx_string_helper::base64_encode(((vsx_widget_base_edit*)editor)->get_string())+" "+vsx_string_helper::f2s(font_size));
  parent->vsx_command_queue_b(this);
}

bool vsx_widget_note::load_text(vsx_string<>new_text) {
  ((vsx_widget_base_edit*)editor)->set_string(new_text);
  return true;
}

void vsx_widget_note::event_move_scale()
{
  ((vsx_widget_base_edit*)editor)->updates++;
}
