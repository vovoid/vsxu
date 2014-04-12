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


#include <map>
#include <list>
#include <vector>
#include <math.h>
#include "vsx_gl_global.h"
#include "vsx_texture_info.h"
#include "vsx_texture.h"
#include "vsx_command.h"
#include "vsx_font.h"
#include "vsxfst.h"
#include "vsx_mouse.h"
#include "vsx_param.h"
#include "vsx_module.h"
#include <vsx_string_aux.h>

// local includes
#include "vsx_widget_base.h"
#include "vsx_widget_sequence.h"
#include "vsx_widget_sequence_tree.h"
#include "vsx_widget_seq_chan.h"
#include "vsx_widget_timeline.h"
#include "vsx_widget_button.h"
#include "vsx_widget_popup_menu.h"
#include "dialogs/vsx_widget_window_statics.h"
#include <gl_helper.h>

#define VSX_ENGINE_STOPPED 0
#define VSX_ENGINE_PLAYING 1
#define VSX_ENGINE_REWIND 2



void vsx_widget_sequence_editor::init()
{
  update_time_from_engine = true;
  tstart = -1;
  curtime = 0;
  tend = 20;

  support_interpolation = true;
  allow_resize_x = true;
  allow_resize_y = true;
  set_size(vsx_vector<>(1.0f,0.5f));
  size_min.x = 0.2;
  size_min.y = 0.2;
  target_pos = pos = camera.get_pos_2d() + vsx_vector<>(0.25);
  camera.set_distance(1.9);

  engine_status = VSX_ENGINE_STOPPED;

  channels_start = 0;

  timeline = add(new vsx_widget_timeline,name+".timeline");
  timeline->init();
  timeline->set_size(vsx_vector<>(size.x*0.995f,size.y*0.04f));
  ((vsx_widget_timeline*)timeline)->owner = this;

  but_rew = add(new vsx_widget_button,name+"rewind");
  but_rew->render_type = VSX_WIDGET_RENDER_3D;
  but_rew->init();
  but_rew->title = "|<";
  but_rew->target_size.x = but_rew->size.x = 0.015;
  but_rew->commands.adds(4,"rewind","rewind","");

  but_play = add(new vsx_widget_button,name+"play");
  but_play->render_type = VSX_WIDGET_RENDER_3D;
  but_play->init();
  but_play->title = ">";
  but_play->target_size.x = but_play->size.x = 0.015;
  but_play->commands.adds(4,"play","play","");

  but_stop = add(new vsx_widget_button,name+"stop");
  but_stop->render_type = VSX_WIDGET_RENDER_3D;
  but_stop->init();
  but_stop->title = "[]";
  but_stop->target_size.x = but_stop->size.x = 0.015;
  but_stop->commands.adds(4,"stop","stop","");
  if (!disable_master_channel)
  {
    but_add_master_channel = add(new vsx_widget_button,name+"stop");
    but_add_master_channel->render_type = VSX_WIDGET_RENDER_3D;
    but_add_master_channel->init();
    but_add_master_channel->title = "master channel+";
    but_add_master_channel->target_size.x = but_add_master_channel->size.x = 0.045;
    but_add_master_channel->commands.adds(4,"button_add_master","button_add_master","");
  }

  but_set_loop_point= add(new vsx_widget_button,"but_set_loop_point");
  but_set_loop_point->render_type = VSX_WIDGET_RENDER_3D;
  but_set_loop_point->init();
  but_set_loop_point->title = "end loop point";
  but_set_loop_point->target_size.x = but_set_loop_point->size.x = 0.040;
  but_set_loop_point->commands.adds(4,"but_set_loop_point","but_set_loop_point","");

  vsx_widget_sequence_tree* sequence_tree = (vsx_widget_sequence_tree*)add(new vsx_widget_sequence_tree, "sequence_list");
  sequence_tree->init();
  sequence_tree->coord_type = VSX_WIDGET_COORD_CORNER;
  sequence_tree->set_pos(vsx_vector<>(size.x/2,size.y/2)-dragborder*2);
  sequence_tree->editor->set_font_size(0.008f);
  sequence_tree->size_from_parent = true;
  sequence_tree->editor->editing_enabled = false;
  sequence_tree->editor->selected_line_highlight = true;
  sequence_tree->editor->enable_syntax_highlighting = false;
  sequence_tree->editor->enable_line_action_buttons = true;
  sequence_tree->pos_from_parent = true;
  sequence_tree->extra_init();
  sequence_tree->set_sequence_editor( this );
  sequence_list = (vsx_widget*)sequence_tree;
  this->interpolate_size();

  set_render_type(render_type);

  title = "sequencer";

  // Menu
  menu = add(new vsx_widget_popup_menu,".comp_menu");
  menu->commands.adds(VSX_COMMAND_MENU, "close", "menu_close","");
  menu->size.x = size.x*0.4;
  menu->init();

  // Name Dialog
  name_dialog = add(new dialog_query_string("name of channel","Choose a unique name for your Master Channel"),"dialog_add");
  ((dialog_query_string*)name_dialog)->set_allowed_chars("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890-_");

  // Loop Time Dialog
  loop_point_dialog = add(new dialog_query_string("loop point in seconds","-1.0 to disable, 2.0 for 2 seconds etc"),"dialog_set_loop_point");
  ((dialog_query_string*)loop_point_dialog)->set_allowed_chars("0123456789-.");
  ((dialog_query_string*)loop_point_dialog)->set_value("1.0");

  load_sequence_list();
}

void vsx_widget_sequence_editor::clear_sequencer()
{
  ((vsx_widget_sequence_tree*)sequence_list)->set_string("[none defined]");
  for (std::vector<vsx_widget*>::iterator it = channels.begin(); it != channels.end(); it++) {
    (*it)->_delete();
  }
  channels.clear();
  channels_map.clear();
}

void vsx_widget_sequence_editor::load_sequence_list()
{
  command_q_b.add_raw("seq_list");
  parent->vsx_command_queue_b(this);
}

void vsx_widget_sequence_editor::i_draw()
{
  parentpos = get_pos_p();
  glBegin(GL_QUADS);
    glColor4f(skin_colors[1].r,skin_colors[1].g,skin_colors[1].b,skin_colors[1].b);
    glVertex3f(parentpos.x-size.x*0.5f, parentpos.y+size.y*0.5f,pos.z);
    glVertex3f(parentpos.x+size.x*0.5f, parentpos.y+size.y*0.5f,pos.z);
    glVertex3f(parentpos.x+size.x*0.5f, parentpos.y+-size.y*0.5f,pos.z);
    glVertex3f(parentpos.x-size.x*0.5f, parentpos.y+-size.y*0.5f,pos.z);
  glEnd();
  glColor4f(skin_colors[0].r,skin_colors[0].g,skin_colors[0].b,skin_colors[0].a);
  draw_box_border(vsx_vector<>(parentpos.x-size.x*0.5,parentpos.y-size.y*0.5f), vsx_vector<>(size.x,size.y), dragborder);

  font.color = vsx_color<>(1.0f,1.0f,1.0f,0.7f);
  font.print(vsx_vector<>(parentpos.x-size.x*0.5+0.3f,parentpos.y+size.y*0.5f-0.015f), name,0.01);
  font.color = vsx_color<>(1.0f,1.0f,1.0f,1.0f);

  vsx_widget::i_draw();
}

void vsx_widget_sequence_editor::update_list()
{
  vsx_string result;
  int i = 0;
  for (std::map<vsx_string,vsx_widget*>::iterator it = channels_map.begin(); it != channels_map.end(); it++)
  {
    if (i++) result += "\n";
    result += (*it).first;
  }
  ((vsx_widget_sequence_tree*)sequence_list)->set_string(result);
}

void vsx_widget_sequence_editor::interpolate_size()
{
  vsx_widget::interpolate_size();
  sequence_list->set_pos(vsx_vector<>(-size.x/2+0.05f+dragborder));
  sequence_list->set_size(vsx_vector<>(0.1f,size.y-dragborder*2));
  but_rew->set_pos(vsx_vector<>( -size.x * 0.5f + 0.1f + but_rew->size.x*0.5f + dragborder*2 ,size.y*0.5f-but_rew->size.x*0.5f-dragborder*2));
  but_play->set_pos(vsx_vector<>(but_rew->pos.x+but_rew->size.x+dragborder, but_rew->pos.y));
  but_stop->set_pos(vsx_vector<>(but_play->pos.x + but_play->size.x + dragborder, but_rew->pos.y));
  but_set_loop_point->set_pos(vsx_vector<>(-size.x*0.5f +0.23f,but_rew->pos.y));
  if (but_add_master_channel) but_add_master_channel->set_pos(vsx_vector<>(but_stop->pos.x + but_stop->size.x  + but_add_master_channel->size.x / 2, but_rew->pos.y));
  timeline->target_size.x = timeline->size.x = size.x-0.1f-dragborder*4;
  timeline->pos.x = timeline->target_pos.x = 0.05f;
  timeline->target_pos.y = timeline->pos.y = but_rew->pos.y-dragborder-but_rew->size.y*0.5-timeline->size.y*0.5f;

  channels_visible = (size.y-timeline->size.y*2.5-0.001f)/0.052f;
  float ypos = size.y*0.5-dragborder*2-but_rew->size.y-timeline->size.y-dragborder;//-0.027-0.025;
  for (int i = 0; i < channels_start; ++i) channels[i]->visible = false;
  unsigned long i = channels_start;

  while (i < channels.size())
  {
    if (((vsx_widget_seq_channel*)channels[i])->hidden_by_sequencer)
    {
      channels[i]->visible = false;
    } else
    {
      if (ypos-channels[i]->size.y > -size.y*0.5)
      {
        channels[i]->size.x = timeline->size.x;
        if (render_type == VSX_WIDGET_RENDER_2D)
          channels[i]->size.y = 0.16f;
        channels[i]->target_size = channels[i]->size;
        channels[i]->pos.x = 0.05f;
        channels[i]->pos.y = ypos-channels[i]->size.y*0.5;
        channels[i]->target_pos = channels[i]->pos;
        ypos -= channels[i]->size.y+dragborder;
        channels[i]->visible = true;
      } else {
        channels[i]->visible = false;
      }
    }
    ++i;
  }
}

void vsx_widget_sequence_editor::toggle_channel_visible(vsx_string name) {
  if (channels_map.find(name) != channels_map.end())
  {
    ((vsx_widget_seq_channel*)channels_map[name])->hidden_by_sequencer = !((vsx_widget_seq_channel*)channels_map[name])->hidden_by_sequencer;
  }
  else {
    // we need to get this from the engine again
    vsx_string deli = ":";
    vsx_avector<vsx_string> parts;
    explode(name,deli,parts);
    if (parts.size())
    {
      if (parts[1] == "[master]")
      {
        command_q_b.add_raw("mseq_channel inject_get "+parts[0]);
      }
      else
      {
        command_q_b.add_raw("pseq_p inject_get "+parts[0]+" "+parts[1]);
      }
    }
    parent->vsx_command_queue_b(this);
  }
  interpolate_size();
}

void vsx_widget_sequence_editor::check_timeline() {
    if (curtime < tstart) {
      float dd = tstart - curtime;
      tstart -= dd;
      tend -= dd;
    } else
    if (curtime > tend) {
      float dd = curtime - tend;
      tstart += dd;
      tend += dd;
    }
    float a = (curtime-tstart)/(tend-tstart);
    if (a > 0.8) {
      float dd = (a-0.8)*(tend-tstart);
      tend += dd;
      tstart += dd;
    } else
    if (a < 0.1) {
      float dd = ((1-a*4)*0.25)*(tend-tstart);
      tend -= dd;
      tstart -= dd;
    }
}

void vsx_widget_sequence_editor::set_timeline_show_wave_data(bool value)
{
  ((vsx_widget_timeline*)timeline)->show_wave_data = value;
}

void vsx_widget_sequence_editor::set_server(vsx_widget* new_server)
{
  server = new_server;
}

vsx_widget* vsx_widget_sequence_editor::get_server()
{
  return server;
}

void vsx_widget_sequence_editor::remove_master_channel_items_with_name(vsx_string name)
{
  for (std::vector<vsx_widget*>::iterator it = channels.begin(); it != channels.end(); it++)
  {
    ((vsx_widget_seq_channel*)(*it))->remove_master_channel_items_with_name(name);
  }
}

void vsx_widget_sequence_editor::command_process_back_queue(vsx_command_s *t) {
  // internal operations
  if (t->cmd == "seq_pool")
  {
    if (t->parts[1] == "del")
    {
      remove_master_channel_items_with_name(t->parts[2]);
    }
  } else
  if (t->cmd == "menu_close")
  {
    command_q_b.add_raw("delete_sequencer");
    parent->vsx_command_queue_b(this);
  }
  else
  if (t->cmd == "button_add_master")
  {
    ((dialog_query_string*)name_dialog)->show();
    return;
  }
  else
  if (t->cmd == "dialog_add")
  {
    command_q_b.add_raw("mseq_channel add " + t->parts[1]);
    parent->vsx_command_queue_b(this);
  }
  if (t->cmd == "but_set_loop_point")
  {
    ((dialog_query_string*)loop_point_dialog)->show();
    return;
  }
  else
  if (t->cmd == "dialog_set_loop_point")
  {
    command_q_b.add_raw("time_set_loop_point " + t->parts[1]);
    parent->vsx_command_queue_b(this);
  }
  else
  if (t->cmd == "editor_action")
  {
    if (t->parts.size() == 3)
    {
      vsx_string deli = ":";
      std::vector<vsx_string> parts;
      explode(t->parts[2],deli, parts);
      if (parts[1] == "[master]")
      {
        backwards_message("mseq_channel remove " + parts[0] );
      }
      else
      {
        backwards_message("pseq_p remove " + parts[0] + " " + parts[1]);
      }
    }
  } else
  if (t->cmd == "remove_chan")
  {
    t->parse();
    bool run = true;
    channels_map.erase(t->parts[1]+":"+t->parts[2]);
    std::vector<vsx_widget*>::iterator it = channels.begin();
    while (run)
    {
      if (((vsx_widget_seq_channel*)(*it))->channel_name == t->parts[1] && ((vsx_widget_seq_channel*)(*it))->param_name == t->parts[2])
      {
        run = false;
        (*it)->_delete();
        channels.erase(it);
        interpolate_size();
      }
      else
      ++it;
    }
  } else
  if (t->cmd == "clear")
  {
    clear_sequencer();
  }
  else
  if (t->cmd == "seq_list_ok") {
    if (t->parts.size() > 1)
    {
      vsx_string deli = "&";
      vsx_avector<vsx_string> parts;
      explode(t->parts[1], deli, parts);
      std::map<vsx_string,vsx_string> sort_map;
      for (size_t i = 0; i < parts.size(); i++)
      {
        sort_map[parts[i]] = parts[i];
      }
      vsx_avector<vsx_string> sorted_names;
      for (std::map<vsx_string,vsx_string>::iterator it = sort_map.begin(); it != sort_map.end(); it++)
      {
        sorted_names.push_back((*it).second);
      }
      deli = "\n";
      ((vsx_widget_sequence_tree*)sequence_list)->set_string(implode(sorted_names,deli));
    } else ((vsx_widget_sequence_tree*)sequence_list)->set_string("[none defined]");
  } else
  // sequence list operations
  if (t->cmd == "pseq_l_dump_ok") {
    if (t->parts.size() > 1)
    {
      vsx_string deli = "&";
      vsx_avector<vsx_string> parts;
      explode(t->parts[1], deli, parts);
      vsx_avector<vsx_string> names;
      for (size_t i = 0; i < parts.size(); i++)
      {
        vsx_string ideli = "#";
        vsx_avector<vsx_string> iparts;
        explode(parts[i], ideli, iparts);
        // 0 = comp name
        // 1 = param name
        // 2 = sequence
        names.push_back(iparts[0]+":"+iparts[1]);
      }
      deli = "\n";
      ((vsx_widget_sequence_tree*)sequence_list)->set_string(implode(names,deli));
    }
  } else

  // pattern operations
  if (t->cmd == "pseq_p_ok") {
    bool show_after_init = false;
    if (t->parts[1] == "inject_get") {
      if (channels_map.find(t->parts[2]+":"+t->parts[3]) != channels_map.end()) {
        command_q_b.add(t);
        channels_map[(t->parts[2]+":"+t->parts[3])]->vsx_command_queue_b((vsx_widget*)this);
      } else {
        t->parts[1] = "init";
        show_after_init = true;
      }
    }
    if (t->parts[1] == "list" || t->parts[1] == "init") {
      if (channels_map.find(t->parts[2]+":"+t->parts[3]) == channels_map.end()) {
        vsx_widget* new_sequence_channel = add(new vsx_widget_seq_channel,".chan");
        new_sequence_channel->size.x = size.x*0.995f;
        new_sequence_channel->size.y = 0.05f;
        new_sequence_channel->target_size = new_sequence_channel->size;
        ((vsx_widget_seq_channel*)new_sequence_channel)->owner = this;
        ((vsx_widget_seq_channel*)new_sequence_channel)->channel_type = VSX_WIDGET_SEQ_CHANNEL_TYPE_PARAMETER;
        ((vsx_widget_seq_channel*)new_sequence_channel)->channel_name = t->parts[2]; // name of channel / module
        ((vsx_widget_seq_channel*)new_sequence_channel)->param_name = t->parts[3];
        new_sequence_channel->init();
        if (show_after_init) ((vsx_widget_seq_channel*)new_sequence_channel)->hidden_by_sequencer = false;
        new_sequence_channel->set_render_type(render_type);
        channels.push_back(new_sequence_channel);
        channels_map[t->parts[2]+":"+t->parts[3]] = new_sequence_channel;
        load_sequence_list();
        interpolate_size();
      }
    } else
    if (t->parts[1] == "remove") {
      bool run = true;
      channels_map.erase(t->parts[2]+":"+t->parts[3]);
      std::vector<vsx_widget*>::iterator it = channels.begin();
      while (run && it != channels.end())
      {
        // if the channel is opened, remove it
        if
        (
            ((vsx_widget_seq_channel*)(*it))->channel_name == t->parts[2]
            && ((vsx_widget_seq_channel*)(*it))->param_name == t->parts[3]
        )
        {
          run = false;
          (*it)->_delete();
          channels.erase(it);
          interpolate_size();
        } else
        ++it;
      }
      // reload the list on the left
      load_sequence_list();
    }
  } else
  // row operations
  if (t->cmd == "pseq_r_ok") {
    // for now, send it to the proper param/sequence, it's not ours to deal with
    if (channels_map.find(t->parts[2]+":"+t->parts[3]) != channels_map.end()) {
      command_q_b.add(t);
      channels_map[(t->parts[2]+":"+t->parts[3])]->vsx_command_queue_b((vsx_widget*)this);
    }
  } else
  if (t->cmd == "time_upd") {
    if (update_time_from_engine) {
      curtime = s2f(t->parts[1]);
      check_timeline();
    }
    engine_status = vsx_string_aux::s2i(t->parts[2]);
  } else
  if (t->cmd == "mseq_channel_ok")
  {
    // 0=mseq_channel_ok 1=row 2=[action] 3=[channel_name] 4=[item_action_id] 5=[local_time_distance] 6=[length]
    if (t->parts[1] == "row") {
      //mseq_channel_ok row insert a 0.626872 -1 0.1
      command_q_b.add(t);
      channels_map[t->parts[3]+":[master]"]->vsx_command_queue_b((vsx_widget*)this);
    }

    //0=mseq_channel_ok 1=inject_get 2=[channel_name] 3=[dump_data]
    if (t->parts[1] == "inject_get") {
      if (channels_map.find(t->parts[2]+":[master]") != channels_map.end())
      {
        command_q_b.add(t);
        channels_map[t->parts[2]+":[master]"]->vsx_command_queue_b((vsx_widget*)this);
      }
      else
      {
        t->parts[1] = "add";
      }
    }
    if (t->parts[1] == "add") {
      vsx_widget* new_sequence_channel = add(new vsx_widget_seq_channel,".chan");
      new_sequence_channel->size.x = size.x*0.995;
      new_sequence_channel->size.y = 0.05;
      new_sequence_channel->target_size = new_sequence_channel->size;
      ((vsx_widget_seq_channel*)new_sequence_channel)->owner = this;
      ((vsx_widget_seq_channel*)new_sequence_channel)->channel_type = VSX_WIDGET_SEQ_CHANNEL_TYPE_MASTER;
      ((vsx_widget_seq_channel*)new_sequence_channel)->channel_name = t->parts[2]; // name of channel
      ((vsx_widget_seq_channel*)new_sequence_channel)->param_name = "[master]";
      new_sequence_channel->init();
      //if (show_after_init)
      ((vsx_widget_seq_channel*)new_sequence_channel)->hidden_by_sequencer = false;
      new_sequence_channel->set_render_type(render_type);
      channels.push_back(new_sequence_channel);
      channels_map[t->parts[2]+":[master]"] = new_sequence_channel;
      //channels.push_back(new_sequence_channel);
      //channels_map[t->parts[2]+":"+t->parts[3]] = w;
      //update_list();
      load_sequence_list();
      interpolate_size();
      new_sequence_channel->visible = 1;
    }
    else
    if (t->parts[1] == "remove")
    {
      if (channels_map.find(t->parts[2]+":[master]") != channels_map.end())
      {
        // GOTCHA!
        vsx_widget_seq_channel* removing_channel = (vsx_widget_seq_channel*)channels_map[t->parts[2]+":[master]"];
        std::vector<vsx_widget*>::iterator it = channels.begin();
        bool run = true;
        while (run)
        {
          if ( (*it) == removing_channel )
          {
            run = false;
            ((vsx_widget_seq_channel*)(*it))->_delete();
            channels.erase(it);
          } else
          {
            ++it;
          }
        }
        channels_map.erase(t->parts[2]+":[master]");
      }
      load_sequence_list();
      interpolate_size();
    }
  } else

  vsx_widget::command_process_back_queue(t);
}

bool vsx_widget_sequence_editor::event_key_down(signed long key, bool alt, bool ctrl, bool shift)
{
  VSX_UNUSED(alt);
  VSX_UNUSED(ctrl);
  VSX_UNUSED(shift);

  switch(key) {
    case 't':
      {
        if (channels_start > 0) --channels_start;
        interpolate_size();
      }
    break;
    case 'g':
      {
        if (channels_start < (long)channels.size()-1) ++channels_start;
        interpolate_size();
      }
    break;
  };
  return true;
}
