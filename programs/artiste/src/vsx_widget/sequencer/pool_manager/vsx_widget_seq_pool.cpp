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

#include <vector>
#include <map>
#include <list>
#include "vsx_param.h"
#include <module/vsx_module.h>
#include "vsx_gl_global.h"
#include <command/vsx_command.h>
#include <command/vsx_command_list.h>
#include <command/vsx_command_client_server.h>
#include "vsx_font.h"
#include "vsx_widget.h"
#include "widgets/vsx_widget_button.h"
#include "vsx_widget_window.h"
#include "widgets/vsx_widget_panel.h"
#include "widgets/vsx_widget_editor.h"
#include "server/vsx_widget_server.h"
#include "server/vsx_widget_comp.h"
#include <texture/vsx_texture.h>

#include "sequencer/vsx_widget_sequence_editor.h"
#include "sequencer/vsx_widget_sequence_channel.h"
#include "vsx_widget_seq_pool.h"
#include <gl_helper.h>

// widget
#include <dialogs/dialog_query_string.h>


class vsx_widget_pool_tree : public vsx_widget_editor {
  std::unique_ptr<vsx_texture<>> mtex_blob;
  //vsx_widget* name_dialog;
  bool dragging;
  vsx_widget_coords drag_coords;
  int mod_i;
  vsx_string<>macro_name;
  vsx_vector2f drop_pos;
  vsx_widget* server;
  int draw_tooltip;
  vsx_string<>tooltip_text;
  vsx_vector3<> tooltip_pos;

public:

  void set_server(vsx_widget* serv)
  {
    server = serv;
  }

  void extra_init()
  {
    dragging = false;
    draw_tooltip = 0;
    editor->mirror_mouse_move_object = this;
    editor->mirror_mouse_move_passive_object = this;
    editor->mirror_mouse_down_object = this;
    editor->mirror_mouse_up_object = this;
    editor->enable_syntax_highlighting = false;

    mtex_blob = vsx_texture_loader::load(
      vsx_widget_skin::get_instance()->skin_path_get() + "interface_extras/connection_blob.png",
      vsx::filesystem::get_instance(),
      false, // threaded
      vsx_bitmap::flip_vertical_hint,
      vsx_texture_gl::linear_interpolate_hint
    );
    set_render_type(render_2d);
  }

  void before_delete()
  {
    mtex_blob.reset(nullptr);
  }

  void event_mouse_move(vsx_widget_distance distance, vsx_widget_coords coords)
  {
    VSX_UNUSED(distance);

    if (get_selected_item() != "")
    {
      dragging = true;
      drag_coords = coords;
    }
  }

  void event_mouse_down(vsx_widget_distance distance,vsx_widget_coords coords,int button)
  {
    VSX_UNUSED(distance);
    VSX_UNUSED(coords);
    VSX_UNUSED(button);

    if (dragging) dragging = false;
    if (get_selected_item() != "")
    {
      command_q_b.add_raw("seq_pool select " + editor->get_line(editor->selected_line) );
      parent->vsx_command_queue_b(this);
    }
  }

  vsx_string<>get_selected_item()
  {
    if (editor->get_line(editor->selected_line) != "[none defined]")
      return editor->get_line(editor->selected_line);
    return "";
  }


  virtual void i_draw()
  {
    vsx_widget_editor::i_draw();

  }

  virtual void draw_2d()// { if (render_type == render_2d) { if (visible) i_draw(); } if (visible) draw_children_2d();}
  {
    i_draw();
    draw_children_2d();
    // draw the little box
    if (dragging && m_focus != editor) dragging = false;
    if (dragging)
    {
      mtex_blob->bind();
      glBlendFunc(GL_SRC_ALPHA, GL_ONE);
      float l_asp = screen_x/screen_y;
      //printf("screen aspect: %f\n",screen_aspect);
      glColor4f(1,1,1,1);
      draw_box_tex_c(drag_coords.screen_global, 0.03/l_asp, 0.03);
      mtex_blob->_bind();
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    if (draw_tooltip && m_o_focus == editor && !dragging) {
      font.color.a = 0.0f;
      vsx_vector3<> sz = font.get_size(tooltip_text, 0.025f);
      glColor4f(0.0f,0.0f,0.0f,0.6f);
      draw_box(vsx_vector3<>(tooltip_pos.x,tooltip_pos.y+0.025*1.05), sz.x, -sz.y);
      glColor4f(1.0f,1.0f,1.0f,0.6f);
      font.color.r = 1.0f;
      font.color.a = 1.0f;
      tooltip_pos.z = 0;
      font.print(tooltip_pos, tooltip_text, 0.022f);

    }
  }

  void event_mouse_up(vsx_widget_distance distance, vsx_widget_coords coords, int button)
  {
    VSX_UNUSED(button);
    if (dragging)
    {
      vsx_widget_distance l_distance;
      vsx_widget* search_widget = root->find_component(coords,l_distance);
      if (search_widget)
      {
        if (search_widget->widget_type == VSX_WIDGET_TYPE_SEQUENCE_CHANNEL)
        {
          drop_pos = l_distance.center;
          if (((vsx_widget_sequence_channel*)search_widget)->channel_type == vsx_widget_sequence_channel_TYPE_MASTER)
          {
            ((vsx_widget_sequence_channel*)search_widget)->drop_master_channel(distance,coords,editor->get_line(editor->selected_line));
          }
        }
      }
      dragging = false;
    }
  }

  void command_process_back_queue(vsx_command_s *t)
  {
    VSX_UNUSED(t);
  }
};

vsx_widget_seq_pool_manager::vsx_widget_seq_pool_manager() {
  // common init
  render_type = render_3d;
    vsx_widget_window::init();
    init_run = true;
    visible = 0;
    title = "Animation Clips";
    allow_resize_x = allow_resize_y = true;
    set_size(vsx_vector3<>(0.15f, 0.7f));
    coord_related_parent = false;
  // tree init
    vsx_widget_pool_tree *e = (vsx_widget_pool_tree*)add(new vsx_widget_pool_tree,"e");
    e->init();
    e->set_string("[none defined]");
    e->set_render_type(render_type);
    e->coord_type = VSX_WIDGET_COORD_CORNER;
    e->set_pos(vsx_vector3<>(size.x/2,size.y/2));
    e->editor->set_font_size(0.016f);
    e->size_from_parent = true;
    e->editor->editing_enabled = false;
    e->editor->selected_line_highlight = true;
    e->set_pos(vsx_vector3<>(size.x/2,size.y/2));
    e->pos_from_parent = true;
    e->extra_init();
    edit = (vsx_widget*)e;
    e->extra_init();

  // buttons
    button_add = add(new vsx_widget_button,"add");
    button_add->init();
    button_add->title = "ADD";
    button_add->commands.adds(4,"add","add","");

    button_del = add(new vsx_widget_button,"del");
    button_del->init();
    button_del->title = "DEL";
    button_del->commands.adds(4,"del","del","");

    button_toggle_edit = add(new vsx_widget_button,"edit");
    button_toggle_edit->init();
    button_toggle_edit->title = "EDIT";
    button_toggle_edit->commands.adds(4,"edit","edit","");

    button_clone = add(new vsx_widget_button,"clone");
    button_clone->init();
    button_clone->title = "CLONE";
    button_clone->commands.adds(4,"clone","clone","");

    button_import = add(new vsx_widget_button,"import");
    button_import->init();
    button_import->title = "IMP";
    button_import->commands.adds(4,"gui_import","gui_import","");

    button_export = add(new vsx_widget_button,"export");
    button_export->init();
    button_export->title = "EXP";
    button_export->commands.adds(4,"gui_export","gui_export","");

    button_propagate = add(new vsx_widget_button,"propagate");
    button_propagate->init();
    button_propagate->title = "PRP";
    button_propagate->commands.adds(4,"propagate_time","propagate_time","");

  // search field
    vsx_widget_base_edit *s = (vsx_widget_base_edit*)add(new vsx_widget_base_edit,"e");
    s->init();
    s->set_font_size(font_size); // 0.02f
    s->size_from_parent = true;
    s->single_row = true;
    s->set_string("");
    s->caret_goto_end();
    s->allowed_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890!#%&()=+?-_";
    s->mirror_keystrokes_object = this;
    search = (vsx_widget*)s;
  // dialogs init
    name_dialog = add(new dialog_query_string("name of pool","Choose a unique name for your Animation / Clip"),"dialog_add");
    ((dialog_query_string*)name_dialog)->set_allowed_chars(s->allowed_chars);

    clone_name_dialog = add(new dialog_query_string("name of clone pool","Choose a unique name for your clone of the Animation / Clip"),"dialog_clone");
    ((dialog_query_string*)clone_name_dialog)->set_allowed_chars(s->allowed_chars);

    import_name_dialog = add(new dialog_query_string("import animation file","Name of the file (in data/animations/*)"),"dialog_import");
    ((dialog_query_string*)import_name_dialog)->set_allowed_chars(s->allowed_chars);

    export_name_dialog = add(new dialog_query_string("export animation file","Name of the file (in data/animations/*)"),"dialog_export");
    ((dialog_query_string*)export_name_dialog)->set_allowed_chars(s->allowed_chars);
  // general inits
    sequencer = 0;
  // done
}

void vsx_widget_seq_pool_manager::init()
{
  backwards_message("seq_pool dump_names");
}

void vsx_widget_seq_pool_manager::set_server(vsx_widget* serv)
{
  server = serv;
  ((vsx_widget_pool_tree*)edit)->set_server(serv);
}

void vsx_widget_seq_pool_manager::event_text(wchar_t character_wide, char character)
{
  VSX_UNUSED(character_wide);
  VSX_UNUSED(character);

  vsx_string<>filter = ((vsx_widget_base_edit*)search)->get_string();
  ((vsx_widget_editor*)edit)->editor->set_filter_string( filter );
}

bool vsx_widget_seq_pool_manager::event_key_down(uint16_t key)
{
  VSX_UNUSED(key);
  vsx_string<>filter = ((vsx_widget_base_edit*)search)->get_string();
  ((vsx_widget_editor*)edit)->editor->set_filter_string( filter );
  return true;
}

void vsx_widget_seq_pool_manager::show() {
  a_focus = k_focus = edit;
  visible = 1;
  set_pos(vsx_vector3<>(0.15f, 0.0f));
  init();
}

void vsx_widget_seq_pool_manager::show(vsx_string<>value) {
  ((vsx_widget_editor*)edit)->set_string(value);
  ((vsx_widget_editor*)edit)->editor->caret_goto_end();
  show();
}

void vsx_widget_seq_pool_manager::i_draw()
{
  vsx_widget_window::i_draw();
  button_add->set_size(vsx_vector3<>(0.02f,font_size));
  button_del->set_size(vsx_vector3<>(0.02f,font_size));
  button_clone->set_size(vsx_vector3<>(0.02f,font_size));
  button_toggle_edit->set_size(vsx_vector3<>(0.02f,font_size));
  button_import->set_size(vsx_vector3<>(0.02f,font_size));
  button_export->set_size(vsx_vector3<>(0.02f,font_size));
  button_propagate->set_size(vsx_vector3<>(0.02f,font_size));

  button_add->set_pos(vsx_vector3<>(button_add->size.x*0.5+dragborder,size.y-font_size*2.5f));
  button_del->set_pos(vsx_vector3<>(button_add->size.x*1.5+dragborder,size.y-font_size*2.5f));
  button_toggle_edit->set_pos(vsx_vector3<>(button_add->size.x*2.5+dragborder, size.y-font_size*2.5f));
  button_clone->set_pos(vsx_vector3<>(button_add->size.x*3.5+dragborder,size.y-font_size*2.5f));
  button_import->set_pos(vsx_vector3<>(button_add->size.x*4.5+dragborder,size.y-font_size*2.5f));
  button_export->set_pos(vsx_vector3<>(button_add->size.x*5.5+dragborder,size.y-font_size*2.5f));
  button_propagate->set_pos(vsx_vector3<>(button_add->size.x*6.5+dragborder,size.y-font_size*2.5f));

  edit->set_pos(vsx_vector3<>(size.x/2,size.y/2-font_size*1.5f+dragborder*0.5f));//+dragborder*0.5f
  edit->set_size(vsx_vector3<>(size.x-dragborder*2,size.y-font_size*3-dragborder));
  search->set_size(vsx_vector3<>(size.x-dragborder*2, font_size));
  search->set_pos(vsx_vector3<>(size.x/2,size.y-font_size*1.5f));
}

void vsx_widget_seq_pool_manager::command_process_back_queue(vsx_command_s *t)
{
  // MESSAGES FROM THE ENGINE
  if (t->cmd == "seq_pool")
  {
    if (t->parts[1] == "del")
    {

    }
    if (t->parts[1] == "dump_names")
    {
      if (t->parts.size() > 2)
      {
        ((vsx_widget_pool_tree*)edit)->set_string(vsx_string_helper::str_replace<char>(";", "\n", t->parts[2]));
      } else
      {
        ((vsx_widget_pool_tree*)edit)->set_string("[none defined]");
      }
    } else
    if (t->parts[1] == "clear_sequencer")
    {
      if (sequencer)
      {
        ((vsx_widget_sequence_editor*)sequencer)->clear_sequencer();
        ((vsx_widget_sequence_editor*)sequencer)->load_sequence_list();
      }
    } else
    if
    (
      t->parts[1] == "pseq_p_ok"
      ||
      t->parts[1] == "pseq_r_ok"
      ||
      t->parts[1] == "pseq_l_dump_ok"
      ||
      t->parts[1] == "seq_list_ok"
      ||
      t->parts[1] == "group_list_ok"
      ||
      t->parts[1] == "time_upd"
    )
    {
      if (sequencer)
      {
        vsx_string<>command;
        for (unsigned long i = 1; i < t->parts.size(); i++)
        {
          if (i != 1) command += " ";
          command += t->parts[i];
        }
        command_q_b.add_raw(command);
        sequencer->vsx_command_queue_b(this,true);
      }
    } else
    if (t->parts[1] == "toggle_edit")
    {
      if (t->parts[2] == "1")
      {
        //sequencer init
        sequencer = (vsx_widget*)add(new vsx_widget_sequence_editor,"Animation Sequencer");


        // don't display master channel button
        ((vsx_widget_sequence_editor*)sequencer)->disable_master_channel = true;
        sequencer->constrained_y = sequencer->constrained_x = false;
        sequencer->coord_related_parent = false;
        sequencer->set_pos(vsx_vector3<>(0.0f,-0.6f));
        sequencer->init();
        ((vsx_widget_sequence_editor*)sequencer)->set_server( server );

      } else
      {
        if (sequencer)
        {
          sequencer->_delete();
          sequencer = 0;
        }
      }
    } else
    command_q_b.add(t);
  } else


  if (t->cmd == "time_set_speed")
  {
    command_q_b.add(t);
    parent->vsx_command_queue_b(this);
  }


  // MESSAGES FOR THE ENGINE
  if
  (
    t->cmd == "stop"
    ||
    t->cmd == "rewind"
    ||
    t->cmd == "play"
    ||
    t->cmd == "time_set_loop_point"
    ||
    t->cmd == "time_set"
    ||
    t->cmd == "pseq_p"
    ||
    t->cmd == "pseq_r"
    ||
    t->cmd == "pseq_l_dump"
    ||
    t->cmd == "seq_list"
    ||
    t->cmd == "group_list"
    ||
    t->cmd == "group_del"
    ||
    t->cmd == "propagate_time"
    ||
    t->cmd == "pseq_inject_get_keyframe_at_time"
  )
  {
    vsx_string<>command = "seq_pool";
    for (unsigned long i = 0; i < t->parts.size(); i++)
    {
      command += " ";
      command += t->parts[i];
    }
    command_q_b.add_raw(command);
    parent->vsx_command_queue_b(this);
  }
  else





  // GUI COMMANDS
  if (t->cmd == "cancel") {
    command_q_b.add(name+"_cancel","cancel");
    parent->vsx_command_queue_b(this);
    visible = 0;
    return;
  } else
  if (t->cmd == "add") {
    ((dialog_query_string*)name_dialog)->show();
    return;
  } else
  if (t->cmd == "dialog_add") {
    command_q_b.add_raw("seq_pool add " + t->parts[1]);
    parent->vsx_command_queue_b(this);
  } else
  if (t->cmd ==	"dialog_clone") {
    command_q_b.add_raw("seq_pool clone " + ((vsx_widget_pool_tree*)edit)->get_selected_item()+ " " + t->parts[1]);
    parent->vsx_command_queue_b(this);
  } else
  if (t->cmd ==	"dialog_import") {
    command_q_b.add_raw("seq_pool import " + t->parts[1]);
    parent->vsx_command_queue_b(this);
  } else
  if (t->cmd ==	"dialog_export") {
    command_q_b.add_raw("seq_pool export " + t->parts[1]);
    parent->vsx_command_queue_b(this);
  } else
  if (t->cmd == "edit")
  {
    backwards_message("seq_pool toggle_edit");
  } else
  if (t->cmd == "clone")
  {
    ((dialog_query_string*)clone_name_dialog)->show();
    return;
  }	else
  if (t->cmd == "gui_import")
  {
    ((dialog_query_string*)import_name_dialog)->show();
  return;
  }	else
  if (t->cmd == "gui_export")
  {
    ((dialog_query_string*)export_name_dialog)->show();
    return;
  }	else
  if (t->cmd == "clear")
  {
    if (sequencer)
    {
      sequencer->_delete();
      sequencer = 0;
    }
    ((vsx_widget_pool_tree*)edit)->set_string("[none defined]");
  } else
  if (t->cmd == "del")
  {
    vsx_string<>del_name = ((vsx_widget_pool_tree*)edit)->get_selected_item();
    if (del_name != "")
    {
      backwards_message("seq_pool del "+del_name);
    }
    return;
  } else
  if (t->cmd == "close")
  {
    visible = 0;
  }
}
