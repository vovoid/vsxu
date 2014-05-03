/**
* Project: VSXu: Realtime modular visual programming language, music/audio visualizer.
*
* This file is part of Vovoid VSXu.
*
* @author Jonatan Wallmander, Vovoid Media Technologies AB Copyright (C) 2014
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

// Globals
#include <gl_helper.h>

// Local libs
#include "vsx_profiler_consumer.h"

// Generic Widget
#include "vsx_widget.h"
#include "widgets/vsx_widget_popup_menu.h"

// Local widgets
#include "vsx_widget_profiler.h"
#include "vsx_widget_profiler_tree.h"
#include "vsx_widget_profiler_timeline.h"

void vsx_widget_profiler::init()
{
  support_interpolation = true;
  allow_resize_x = true;
  allow_resize_y = true;
  set_size(vsx_vector<>(2.0f,1.5f));
  size_min.x = 0.2;
  size_min.y = 0.2;
  target_pos = pos = camera.get_pos_2d() + vsx_vector<>(0.25);
  camera.set_distance(2.9);

  // Init Timeline
//  timeline_window = add(new vsx_widget_profiler_timeline_window, name+".timeline");
//  timeline_window->init();
//  timeline_window->set_size(vsx_vector<>(0.2,0.1));
//  timeline_window->set_render_type( render_2d );
//  ((vsx_widget_profiler_timeline_window*)timeline_window)->time_holder_set( &time );

  // Init File List
  vsx_widget_profiler_tree_window* profile_tree = (vsx_widget_profiler_tree_window*)add(new vsx_widget_profiler_tree_window, "profile_list");
  profile_tree->init();
  profile_tree->set_size(vsx_vector<>(0.15,1.0));
  profile_tree->set_pos( vsx_vector<>(0.0, 0.0) );
  profile_tree->show();
  profile_tree->set_render_type(render_2d);



//  profile_tree->coord_type = VSX_WIDGET_COORD_CORNER;
//  profile_tree->set_pos(vsx_vector<>(size.x/2,size.y/2)-dragborder*2);
//  profile_tree->editor->set_font_size(0.008f);
//  profile_tree->size_from_parent = true;
//  profile_tree->editor->editing_enabled = false;
//  profile_tree->editor->selected_line_highlight = true;
//  profile_tree->editor->enable_syntax_highlighting = false;
//  profile_tree->editor->enable_line_action_buttons = true;
//  profile_tree->pos_from_parent = true;
  profile_tree->extra_init();
  profile_tree->set_profiler( this );
//



  profile_tree->set_string( vsx_profiler_consumer::get_instance()->get_filenames() );
  file_list = (vsx_widget*)profile_tree;

  // Menu
  menu = add(new vsx_widget_popup_menu, ".comp_menu");
  menu->commands.adds(VSX_COMMAND_MENU, "close", "menu_close", "");
  menu->size.x = size.x * 0.4;
  menu->init();

  // make sure interpolation is called
  this->interpolate_size();
  init_run = true;
}

void vsx_widget_profiler::update_list()
{

}

void vsx_widget_profiler::i_draw()
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
  draw_box_border(vsx_vector<>(parentpos.x-size.x*0.5,parentpos.y-size.y*0.5f), vsx_vector<>(size.x,size.y), dragborder);

  vsx_widget::i_draw();
}

void vsx_widget_profiler::load_profile(int id)
{
  vsx_printf("load profile %d\n", id);
  vsx_profiler_consumer::get_instance()->load_profile((size_t)id);

  vsx_profiler_consumer::get_instance()->get_chunks(0.0, 5.0, consumer_chunks);
}

void vsx_widget_profiler::command_process_back_queue(vsx_command_s *t)
{
  if (t->cmd == "menu_close")
  {
    _delete();
    return;
  }

  vsx_printf("t->cmd: %s\n", t->cmd_data.c_str());

}

bool vsx_widget_profiler::event_key_down(signed long key, bool alt, bool ctrl, bool shift)
{
  VSX_UNUSED(alt);
  VSX_UNUSED(ctrl);
  VSX_UNUSED(shift);
  return true;
}

void vsx_widget_profiler::interpolate_size()
{
  vsx_widget::interpolate_size();
//  file_list->set_pos(vsx_vector<>(-size.x/2+0.05f+dragborder));
//  file_list->set_size(vsx_vector<>(0.1f,size.y-dragborder*2));

//  timeline_window->target_size.x = timeline_window->size.x = size.x-0.1f-dragborder*4;
//  timeline_window->pos.x = timeline_window->target_pos.x = 0.05f;
//  timeline_window->target_pos.y = timeline_window->pos.y =  size.y*0.5f - dragborder-timeline_window->size.y*0.5f;

//  float ypos = size.y*0.5-dragborder*2-but_rew->size.y-timeline->size.y-dragborder;//-0.027-0.025;
//  for (int i = 0; i < channels_start; ++i) channels[i]->visible = false;
//  unsigned long i = channels_start;

//  while (i < channels.size())
//  {
//    if (((vsx_widget_seq_channel*)channels[i])->hidden_by_sequencer)
//    {
//      channels[i]->visible = false;
//    } else
//    {
//      if (ypos-channels[i]->size.y > -size.y*0.5)
//      {
//        channels[i]->size.x = timeline->size.x;
//        if (render_type == render_2d)
//          channels[i]->size.y = 0.16f;
//        channels[i]->target_size = channels[i]->size;
//        channels[i]->pos.x = 0.05f;
//        channels[i]->pos.y = ypos-channels[i]->size.y*0.5;
//        channels[i]->target_pos = channels[i]->pos;
//        ypos -= channels[i]->size.y+dragborder;
//        channels[i]->visible = true;
//      } else {
//        channels[i]->visible = false;
//      }
//    }
//    ++i;
//  }
}
