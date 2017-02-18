/**
* Project: VSXu Profiler - Data collection and data visualizer
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
#include <profiler/vsx_profiler_consumer.h>

// Generic Widget
#include "vsx_widget.h"
#include "widgets/vsx_widget_popup_menu.h"

// Local widgets
#include "vsx_widget_profiler.h"
#include "vsx_widget_profiler_tree.h"
#include "vsx_widget_profiler_items.h"
#include "vsx_widget_profiler_timeline.h"
#include "vsx_widget_profiler_thread.h"
#include "vsx_widget_profiler_plot.h"
#include "time_scale.h"
#include "cursor.h"




void vsx_widget_profiler::init()
{
  support_interpolation = true;
  allow_resize_x = true;
  allow_resize_y = true;
  set_size(vsx_vector3<>(20.0f,20.0f));
  size_min.x = 0.2;
  size_min.y = 0.2;

  title = "select profile";

  allow_move_x = false;
  allow_move_y = false;




  profiler = vsx_profiler_manager::get_instance()->get_profiler();

  target_pos = pos = camera.get_pos_2d() + vsx_vector3<>(0.25);
  camera.set_distance(2.9);

  // Init Timeline
//  timeline_window = add(new vsx_widget_profiler_timeline_window, name+".timeline");
//  timeline_window->init();
//  timeline_window->set_size(vsx_vector3<>(0.2,0.1));
//  timeline_window->set_render_type( render_2d );
//  ((vsx_widget_profiler_timeline_window*)timeline_window)->time_holder_set( &time );

  // Init File List
  vsx_widget_profiler_tree_window* profile_tree = (vsx_widget_profiler_tree_window*)add(new vsx_widget_profiler_tree_window, "profile_list");
  profile_tree->init();
  profile_tree->set_size(vsx_vector3<>(0.15,1.0));
  profile_tree->set_pos( vsx_vector3<>(0.0, 0.0) );
  profile_tree->show();
  profile_tree->set_render_type(render_2d);
  profile_tree->extra_init();
  profile_tree->set_profiler( this );
  profile_tree->set_string( vsx_profiler_consumer::get_instance()->get_filenames_list() );
  file_list = (vsx_widget*)profile_tree;

  // Init Item List
  vsx_widget_profiler_items_window* items = (vsx_widget_profiler_items_window*)add(new vsx_widget_profiler_items_window, "item list");
  items->init();
  items->set_size(vsx_vector3<>(0.10,1.0));
  items->set_pos( vsx_vector3<>(1.0- 0.10, 0.0) );
  items->show();
  items->set_render_type(render_2d);
  items->extra_init();
  items->set_profiler( this );
  item_list = (vsx_widget*)items;

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

void vsx_widget_profiler::load_profile(int id)
{
  for (size_t i = 0; i < threads.size(); i++)
  {
    threads[i]->_delete();
  }
  threads.reset_used();

  for (size_t i = 0; i < plots.size(); i++)
  {
    plots[i]->_delete();
  }
  plots.reset_used();

  vsx_profiler_consumer::get_instance()->load_profile(id);

  ((vsx_widget_profiler_items_window*)item_list)->set_string( vsx_profiler_consumer::get_instance()->get_items_list() );


//  vsx_widget_profiler_thread* thread = (vsx_widget_profiler_thread*)add( new vsx_widget_profiler_thread, "thread" );
//  thread->init();
//  thread->load_profile(id);
//  threads.push_back(thread);
}

void vsx_widget_profiler::load_thread(uint64_t id)
{
  vsx_widget_profiler_thread* thread = (vsx_widget_profiler_thread*)add( new vsx_widget_profiler_thread, "thread" );
  thread->init();
  thread->load_thread(id);
  threads.push_back(thread);
}

void vsx_widget_profiler::load_plot(size_t id)
{
  vsx_widget_profiler_plot* plot = (vsx_widget_profiler_plot*)add( new vsx_widget_profiler_plot, "plot" );
  plot->init();
  plot->load_plot(id);
  plots.push_back(plot);

}

void vsx_widget_profiler::i_draw()
{
  vsx_vector3<> parentpos = get_pos_p();
  glBegin(GL_QUADS);
    vsx_widget_skin::get_instance()->set_color_gl(1);
    glVertex3f(parentpos.x-size.x*0.5f, parentpos.y+size.y*0.5f,pos.z);
    glVertex3f(parentpos.x+size.x*0.5f, parentpos.y+size.y*0.5f,pos.z);
    glVertex3f(parentpos.x+size.x*0.5f, parentpos.y+-size.y*0.5f,pos.z);
    glVertex3f(parentpos.x-size.x*0.5f, parentpos.y+-size.y*0.5f,pos.z);
  glEnd();
  vsx_widget_skin::get_instance()->set_color_gl(0);
  draw_box_border(vsx_vector3<>(parentpos.x-size.x*0.5,parentpos.y-size.y*0.5f), vsx_vector3<>(size.x,size.y), dragborder);

  glColor4f(1,1,1,1);

  glColor4f(1,1,1,0.25);
  glBegin(GL_LINES);
    glVertex2f(cursor::get_instance()->pos.x, cursor::get_instance()->pos.y - 1000.0);
    glVertex2f(cursor::get_instance()->pos.x, cursor::get_instance()->pos.y + 1000.0);

    glVertex2f(cursor::get_instance()->pos.x - 1000.0, cursor::get_instance()->pos.y);
    glVertex2f(cursor::get_instance()->pos.x + 1000.0, cursor::get_instance()->pos.y);
  glEnd();

  vsx_widget::i_draw();
}


void vsx_widget_profiler::command_process_back_queue(vsx_command_s *t)
{
  if (t->cmd == "menu_close")
  {
    _delete();
    return;
  }

  vsx_printf(L"t->cmd: %s\n", t->cmd_data.c_str());
}

bool vsx_widget_profiler::event_key_down(uint16_t key)
{
  VSX_UNUSED(key);
  return true;
}

void vsx_widget_profiler::event_mouse_move_passive(vsx_widget_distance distance,vsx_widget_coords coords)
{
  VSX_UNUSED(distance);
  profiler->plot_2(0, coords.world_global.x, coords.world_global.y);
  cursor::get_instance()->pos = coords.world_global;
}

void vsx_widget_profiler::update_children()
{
  for (size_t i = 0; i <  threads.size(); i++)
  {
    threads[i]->update();
  }
  for (size_t i = 0; i <  plots.size(); i++)
  {
    plots[i]->update();
  }
}

void vsx_widget_profiler::event_mouse_wheel(float y)
{
  if (vsx_input_keyboard.pressed_shift())
  {
    vsx_printf(L"camera z: %f\n", camera.get_pos_z());

    time_scale::get_instance()->time_offset += 0.025 * y * (camera.get_pos_z() - 1.1);

    update_children();

    return;
  }

  float factor = time_scale::get_instance()->world_to_time_factor( cursor::get_instance()->pos.x );

  float time_scale_pre = time_scale::get_instance()->time_scale_x;

  time_scale::get_instance()->time_scale_x *= 1.0 + chunk_height * y;

  time_scale::get_instance()->time_offset -= factor * ( time_scale::get_instance()->time_scale_x * time_scale::get_instance()->chunk_time_end - time_scale_pre * time_scale::get_instance()->chunk_time_end );

  update_children();
}

void vsx_widget_profiler::interpolate_size()
{
  vsx_widget::interpolate_size();
}
