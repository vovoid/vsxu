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
#include <texture/vsx_texture.h>
#include "vsx_command.h"
#include "vsx_font.h"
#include "vsxfst.h"
#include "vsx_mouse.h"
#include "vsx_param.h"
#include "vsx_module.h"
#include <string/vsx_string_helper.h>

// local includes
#include "vsx_widget.h"
#include "vsx_widget_sequence_editor.h"
#include "vsx_widget_sequence_tree.h"
#include "vsx_widget_seq_chan.h"
#include "vsx_widget_timeline.h"
#include "widgets/vsx_widget_button.h"
#include "widgets/vsx_widget_popup_menu.h"
#include <gl_helper.h>

// widget
#include <dialogs/dialog_query_string.h>

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
  set_size(vsx_vector3<>(1.0f,0.5f));
  size_min.x = 0.2;
  size_min.y = 0.2;
  target_pos = pos = camera.get_pos_2d() + vsx_vector3<>(0.25);
  camera.set_distance(1.9);

  engine_status = VSX_ENGINE_STOPPED;

  channels_start = 0;

  timeline = add(new vsx_widget_timeline,name+".timeline");
  timeline->init();
  timeline->set_size(vsx_vector3<>(size.x*0.995f,size.y*0.04f));
  ((vsx_widget_timeline*)timeline)->owner = this;

  but_rew = add(new vsx_widget_button,name+"rewind");
  but_rew->render_type = render_3d;
  but_rew->init();
  but_rew->title = "|<";
  but_rew->target_size.x = but_rew->size.x = 0.015;
  but_rew->commands.adds(4,"rewind","rewind","");

  but_play = add(new vsx_widget_button,name+"play");
  but_play->render_type = render_3d;
  but_play->init();
  but_play->title = ">";
  but_play->target_size.x = but_play->size.x = 0.015;
  but_play->commands.adds(4,"play","play","");

  but_stop = add(new vsx_widget_button,name+"stop");
  but_stop->render_type = render_3d;
  but_stop->init();
  but_stop->title = "[]";
  but_stop->target_size.x = but_stop->size.x = 0.015;
  but_stop->commands.adds(4,"stop","stop","");
  if (!disable_master_channel)
  {
    but_add_master_channel = add(new vsx_widget_button,name+"stop");
    but_add_master_channel->render_type = render_3d;
    but_add_master_channel->init();
    but_add_master_channel->title = "master channel+";
    but_add_master_channel->target_size.x = but_add_master_channel->size.x = 0.045;
    but_add_master_channel->commands.adds(4,"button_add_master","button_add_master","");
  }

  but_set_loop_point= add(new vsx_widget_button,"but_set_loop_point");
  but_set_loop_point->render_type = render_3d;
  but_set_loop_point->init();
  but_set_loop_point->title = "end loop point";
  but_set_loop_point->target_size.x = but_set_loop_point->size.x = 0.040;
  but_set_loop_point->commands.adds(4,"but_set_loop_point","but_set_loop_point","");

  but_set_speed= add(new vsx_widget_button,"but_set_speed");
  but_set_speed->render_type = render_3d;
  but_set_speed->init();
  but_set_speed->title = "set speed";
  but_set_speed->target_size.x = but_set_speed->size.x = 0.040;
  but_set_speed->commands.adds(4,"but_set_speed","but_set_speed","");

  but_open_at_time= add(new vsx_widget_button,"but_open_at_time");
  but_open_at_time->render_type = render_3d;
  but_open_at_time->init();
  but_open_at_time->title = "open at time";
  but_open_at_time->target_size.x = but_open_at_time->size.x = 0.040;
  but_open_at_time->commands.adds(4,"but_open_at_time","but_open_at_time","");

  but_copy = add(new vsx_widget_button,"but_copy");
  but_copy->render_type = render_3d;
  but_copy->init();
  but_copy->title = "copy";
  but_copy->target_size.x = but_copy->size.x = 0.040;
  but_copy->commands.adds(4,"but_copy","but_copy","");

  but_paste = add(new vsx_widget_button,"but_paste");
  but_paste->render_type = render_3d;
  but_paste->init();
  but_paste->title = "paste";
  but_paste->target_size.x = but_paste->size.x = 0.040;
  but_paste->commands.adds(4,"but_paste","but_paste","");


  vsx_widget_sequence_tree* sequence_tree = (vsx_widget_sequence_tree*)add(new vsx_widget_sequence_tree, "sequence_list");
  sequence_tree->init();
  sequence_tree->extra_init();
  sequence_tree->set_sequence_editor( this );
  sequence_list = (vsx_widget*)sequence_tree;
  this->interpolate_size();
  set_render_type(render_type);
  sequence_tree->init_2d();



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

  // Speed Dialog
  speed_dialog = add(new dialog_query_string("Time progression speed factor","1.0 is normal, only positive values"),"dialog_set_speed");
  ((dialog_query_string*)speed_dialog)->set_allowed_chars("0123456789.");
  ((dialog_query_string*)speed_dialog)->set_value("1.0");

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
  vsx_widget_skin::get_instance()->set_color_gl(1);
    glVertex3f(parentpos.x-size.x*0.5f, parentpos.y+size.y*0.5f,pos.z);
    glVertex3f(parentpos.x+size.x*0.5f, parentpos.y+size.y*0.5f,pos.z);
    glVertex3f(parentpos.x+size.x*0.5f, parentpos.y+-size.y*0.5f,pos.z);
    glVertex3f(parentpos.x-size.x*0.5f, parentpos.y+-size.y*0.5f,pos.z);
  glEnd();
  vsx_widget_skin::get_instance()->set_color_gl(0);

  draw_box_border(vsx_vector3<>(parentpos.x-size.x*0.5,parentpos.y-size.y*0.5f), vsx_vector3<>(size.x,size.y), dragborder);

  font.color = vsx_color<>(1.0f,1.0f,1.0f,0.7f);
//  font.print(vsx_vector3<>(parentpos.x-size.x*0.5+0.3f,parentpos.y+size.y*0.5f-0.018f), name,0.01);
  font.color = vsx_color<>(1.0f,1.0f,1.0f,1.0f);

  vsx_widget::i_draw();
}

void vsx_widget_sequence_editor::update_list()
{
  vsx_string<>result;
  int i = 0;
  for (std::map<vsx_string<>,vsx_widget*>::iterator it = channels_map.begin(); it != channels_map.end(); it++)
  {
    if (i++) result += "\n";
    result += (*it).first;
  }
  ((vsx_widget_sequence_tree*)sequence_list)->set_string(result);
}

void vsx_widget_sequence_editor::interpolate_size()
{
  vsx_widget::interpolate_size();
  sequence_list->set_pos(vsx_vector3<>(-size.x/2+0.05f+dragborder));
  sequence_list->set_size(vsx_vector3<>(0.1f,size.y-dragborder*2));
  but_rew->set_pos(vsx_vector3<>( -size.x * 0.5f + 0.1f + but_rew->size.x*0.5f + dragborder*2 ,size.y*0.5f-but_rew->size.x*0.5f-dragborder*2));
  but_play->set_pos(vsx_vector3<>(but_rew->pos.x+but_rew->size.x+dragborder, but_rew->pos.y));
  but_stop->set_pos(vsx_vector3<>(but_play->pos.x + but_play->size.x + dragborder, but_rew->pos.y));
  but_set_loop_point->set_pos(vsx_vector3<>(-size.x*0.5f +0.23f,but_rew->pos.y));
  but_set_speed->set_pos(vsx_vector3<>(-size.x*0.5f +0.275f,but_rew->pos.y));
  but_open_at_time->set_pos(vsx_vector3<>(-size.x*0.5f +0.275f + 0.045, but_rew->pos.y));
  but_copy->set_pos(vsx_vector3<>(-size.x*0.5f +0.275f + 0.045 * 2, but_rew->pos.y));
  but_paste->set_pos(vsx_vector3<>(-size.x*0.5f +0.275f + 0.045 * 3, but_rew->pos.y));

  if (but_add_master_channel)
    but_add_master_channel->set_pos(vsx_vector3<>(but_stop->pos.x + but_stop->size.x  + but_add_master_channel->size.x / 2, but_rew->pos.y));
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
        if (render_type == render_2d)
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

void vsx_widget_sequence_editor::close_open_channels()
{
  for (std::vector<vsx_widget*>::iterator it = channels.begin(); it != channels.end(); it++)
    (*it)->_delete();

  channels.clear();
  channels_map.clear();
}

void vsx_widget_sequence_editor::channels_open_at_time()
{
  close_open_channels();
  command_q_b.add_raw("pseq_inject_get_keyframe_at_time "+ vsx_string_helper::f2s(curtime) + " 0.1");
  parent->vsx_command_queue_b(this);
  interpolate_size();
}


void vsx_widget_sequence_editor::toggle_channel_visible(vsx_string<>name) {
  if (channels_map.find(name) != channels_map.end())
  {
    ((vsx_widget_seq_channel*)channels_map[name])->hidden_by_sequencer = !((vsx_widget_seq_channel*)channels_map[name])->hidden_by_sequencer;
  }
  else {
    // we need to get this from the engine again
    vsx_string<>deli = ":";
    vsx_nw_vector< vsx_string<> > parts;
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

void vsx_widget_sequence_editor::remove_master_channel_items_with_name(vsx_string<>name)
{
  for (std::vector<vsx_widget*>::iterator it = channels.begin(); it != channels.end(); it++)
    ((vsx_widget_seq_channel*)(*it))->remove_master_channel_items_with_name(name);
}

void vsx_widget_sequence_editor::action_copy()
{
  clipboard.clear();
  for (size_t i = 0; i < channels.size(); i++)
  {
    vsx_widget_seq_channel& channel = *((vsx_widget_seq_channel*)channels[i]);
    float result;
    if ( !channel.get_keyframe_value(curtime, 0.1, result) )
      continue;
    clipboard.add( channel.channel_name, channel.param_name, result );
  }
}

void vsx_widget_sequence_editor::action_paste()
{
  vsx_nw_vector< vsx_widget_sequence_clipboard_item >& items = *clipboard.items_get_ptr();
  for (size_t i = 0; i < items.size(); i++)
  {
    vsx_widget_seq_channel& channel = *((vsx_widget_seq_channel*)channels_map[items[i].component+":"+items[i].parameter]);
    channel.create_keyframe( curtime, items[i].value );
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

  if (t->cmd == "dialog_set_loop_point")
  {
    command_q_b.add_raw("time_set_loop_point " + t->parts[1]);
    parent->vsx_command_queue_b(this);
  }
  else
  if (t->cmd == "but_set_speed")
  {
    ((dialog_query_string*)speed_dialog)->show();
    return;
  }
  if (t->cmd == "dialog_set_speed")
  {
    command_q_b.add_raw("time_set_speed " + t->parts[1]);
    parent->vsx_command_queue_b(this);
  }

  if (t->cmd == "but_open_at_time")
  {
    channels_open_at_time();
    return;
  }

  if (t->cmd == "but_copy")
  {
    action_copy();
    return;
  }

  if (t->cmd == "but_paste")
  {
    action_paste();
    return;
  }


  if (t->cmd == "editor_action")
  {
    if (t->parts.size() == 3)
    {
      vsx_string<>deli = ":";
      vsx_nw_vector<vsx_string<> > parts;
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
      vsx_string<>deli = "&";
      vsx_nw_vector< vsx_string<> > parts;
      explode(t->parts[1], deli, parts);
      std::map<vsx_string<>,vsx_string<> > sort_map;
      for (size_t i = 0; i < parts.size(); i++)
      {
        sort_map[parts[i]] = parts[i];
      }
      vsx_nw_vector< vsx_string<> > sorted_names;
      for (std::map<vsx_string<>,vsx_string<> >::iterator it = sort_map.begin(); it != sort_map.end(); it++)
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
      vsx_string<>deli = "&";
      vsx_nw_vector< vsx_string<> > parts;
      explode(t->parts[1], deli, parts);
      vsx_nw_vector< vsx_string<> > names;
      for (size_t i = 0; i < parts.size(); i++)
      {
        vsx_string<>ideli = "#";
        vsx_nw_vector< vsx_string<> > iparts;
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
  if (t->cmd == "pseq_p_ok")
  {
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
  if (t->cmd == "pseq_r_ok")
  {
    // for now, send it to the proper param/sequence, it's not ours to deal with
    if (channels_map.find(t->parts[2]+":"+t->parts[3]) != channels_map.end())
    {
      command_q_b.add(t);
      channels_map[(t->parts[2]+":"+t->parts[3])]->vsx_command_queue_b((vsx_widget*)this);
    }
  } else
  if (t->cmd == "time_upd") {
    if (update_time_from_engine) {
      curtime = vsx_string_helper::s2f(t->parts[1]);
      check_timeline();
    }
    engine_status = vsx_string_helper::s2i(t->parts[2]);
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
    return;
  }

  if (t->cmd == "save")
  {
    command_q_b.add_raw(
      "pseq_p "
      "save " +
      dynamic_cast<vsx_widget_sequence_tree*>(sequence_list)->get_selected_component() + " " +
      dynamic_cast<vsx_widget_sequence_tree*>(sequence_list)->get_selected_parameter() + " " +
      t->cmd_data
    );
    parent->vsx_command_queue_b(this);
  }


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
