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
  set_size(vsx_vector<>(20.0f,20.0f));
  size_min.x = 0.2;
  size_min.y = 0.2;

  time_scale = 1.0;
  time_offset = 0.0;

  allow_move_x = false;
  allow_move_y = false;


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
  profile_tree->extra_init();
  profile_tree->set_profiler( this );
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

void vsx_widget_profiler::load_profile(int id)
{
  vsx_printf("load profile %d\n", id);
  vsx_profiler_consumer::get_instance()->load_profile((size_t)id);

  vsx_profiler_consumer::get_instance()->get_chunks(0.0, 5.0, consumer_chunks);
  update_vbo();
}

void vsx_widget_profiler::update_vbo()
{
  draw_bucket.vertices.reset_used();
  draw_bucket.faces.reset_used();

  vsx_printf("time scale: %f\n", time_scale);

  for (size_t i = 0; i < consumer_chunks.size(); i++)
  {
    vsx_profiler_consumer_chunk& chunk = consumer_chunks[i];
    float depth = -(chunk.depth + 1.0) * 0.1;

    float cts = (chunk.time_start ) * time_scale + time_offset;
    float cte = (chunk.time_end ) * time_scale + time_offset;

    draw_bucket.vertices.push_back( vsx_vector<>( cts, depth ) );
    draw_bucket.vertices.push_back( vsx_vector<>( cts, depth - 0.1 ) );
    draw_bucket.vertices.push_back( vsx_vector<>( cte, depth - 0.1 ) );
    draw_bucket.vertices.push_back( vsx_vector<>( cte, depth ) );

    chunk_time_end = chunk.time_end;
    time_size_x = chunk.time_end * time_scale;

    line_index idx;
    idx.a = draw_bucket.vertices.size() - 4;
    idx.b = draw_bucket.vertices.size() - 3;
    draw_bucket.faces.push_back( idx );

    idx.a = draw_bucket.vertices.size() - 3;
    idx.b = draw_bucket.vertices.size() - 2;
    draw_bucket.faces.push_back( idx );

    idx.a = draw_bucket.vertices.size() - 2;
    idx.b = draw_bucket.vertices.size() - 1;
    draw_bucket.faces.push_back( idx );
  }
  draw_bucket.invalidate_vertices();
  draw_bucket.update();

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

  glColor4f(1,1,1,1);
  draw_bucket.output();

  glColor4f(1,1,1,0.25);
  glBegin(GL_LINES);
    glVertex2f(mouse_pos.x, mouse_pos.y - 1000.0);
    glVertex2f(mouse_pos.x, mouse_pos.y + 1000.0);

    glVertex2f(mouse_pos.x - 1000.0, mouse_pos.y);
    glVertex2f(mouse_pos.x + 1000.0, mouse_pos.y);
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

  vsx_printf("t->cmd: %s\n", t->cmd_data.c_str());

}

bool vsx_widget_profiler::event_key_down(signed long key, bool alt, bool ctrl, bool shift)
{
  VSX_UNUSED(alt);
  VSX_UNUSED(ctrl);
  VSX_UNUSED(shift);
  return true;
}



void vsx_widget_profiler::event_mouse_wheel(float y)
{
  if (ctrl)
  {
    time_offset += (1.0 / time_scale) * 0.025 * y * fabs(camera.get_pos_z());
    update_vbo();
    return;
  }

  float f = (mouse_pos.x + size.x * 0.5) / (size.x);

  float factor = world_to_time_factor( mouse_pos.x );

  float time_scale_pre = time_scale;

  time_scale *= 1.0 + 0.05 * y;

  time_offset -= factor * ( time_scale * chunk_time_end - time_scale_pre * chunk_time_end );

  update_vbo();

}

void vsx_widget_profiler::interpolate_size()
{
  vsx_widget::interpolate_size();
}
