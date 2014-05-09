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

  title = "select profile";

  allow_move_x = false;
  allow_move_y = false;

  selected_chunk = 0x0;


  profiler = vsx_profiler_manager::get_instance()->get_profiler();

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

    double cts = (chunk.time_start ) * time_scale;
    double cte = (chunk.time_end ) * time_scale;

    if (cte - cts < 0.0001)
      continue;


    float depth = -(chunk.depth + 1.0) * chunk_height;
    cts += time_offset;
    cte += time_offset;

    draw_bucket.vertices.push_back( vsx_vector<>( cts, depth ) );
    draw_bucket.vertices.push_back( vsx_vector<>( cts, depth - chunk_height ) );
    draw_bucket.vertices.push_back( vsx_vector<>( cte, depth - chunk_height ) );
    draw_bucket.vertices.push_back( vsx_vector<>( cte, depth ) );

    draw_bucket.vertex_colors.push_back( vsx_color<>(1.0, 1.0, 1.0, 0.5) );
    draw_bucket.vertex_colors.push_back( vsx_color<>(1.0, 1.0, 1.0, 0.5) );
    draw_bucket.vertex_colors.push_back( vsx_color<>(1.0, 1.0, 1.0, 0.5) );
    draw_bucket.vertex_colors.push_back( vsx_color<>(1.0, 1.0, 1.0, 0.5) );

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

  one_div_chunk_time_end = 1.0 / chunk_time_end;
  one_div_time_size_x = 1.0 / time_size_x;

  draw_bucket.invalidate_vertices();
  draw_bucket.invalidate_colors();
  draw_bucket.update();
}

void vsx_widget_profiler::update_tag_draw_chunks()
{
  tag_draw_chunks.reset_used();

  for (size_t i = 0; i < consumer_chunks.size(); i++)
  {
    if
    (
      consumer_chunks[i].time_start * time_scale + time_offset > -15.0
      &&
      (consumer_chunks[i].time_end * time_scale) - (consumer_chunks[i].time_start * time_scale ) > 0.02
    )
    {
      tag_draw_chunks.push_back( &consumer_chunks[i] );
    }

    if (consumer_chunks[i].time_start * time_scale + time_offset > 15.0)
      break;
  }
}

void vsx_widget_profiler::draw_tags()
{
  for (size_t i = 0; i < tag_draw_chunks.size(); i++)
  {
    vsx_vector<> dpos( tag_draw_chunks[i]->time_start * time_scale + time_offset,  -(tag_draw_chunks[i]->depth + 1) * chunk_height - 0.005 );
    font.print( dpos, tag_draw_chunks[i]->tag, 0.005 );
    dpos.y -= 0.005;
    font.print( dpos, i2s(tag_draw_chunks[i]->cycles_end-tag_draw_chunks[i]->cyclea_start) + " CPU cycles", 0.005 );
    dpos.y -= 0.005;
    font.print( dpos, f2s(tag_draw_chunks[i]->time_end - tag_draw_chunks[i]->time_start) + " seconds", 0.005 );

  }

}


void vsx_widget_profiler::i_draw()
{
  profiler->sub_begin("vwp::i_draw");
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


  glColor4f(0,1,0,1);
  glBegin(GL_LINES);
    glVertex2f( - 1000.0, -chunk_height + 0.001);
    glVertex2f(   1000.0, -chunk_height + 0.001);
  glEnd();

  glColor4f(1,1,1,0.25);
  glBegin(GL_LINES);
    glVertex2f(mouse_pos.x, mouse_pos.y - 1000.0);
    glVertex2f(mouse_pos.x, mouse_pos.y + 1000.0);

    glVertex2f(mouse_pos.x - 1000.0, mouse_pos.y);
    glVertex2f(mouse_pos.x + 1000.0, mouse_pos.y);
  glEnd();

  if (selected_chunk != 0x0)
  {
    float depth = -(selected_chunk->depth + 1.0) * chunk_height;

    float cts = (selected_chunk->time_start ) * time_scale + time_offset;
    float cte = (selected_chunk->time_end )   * time_scale + time_offset;

    glColor4f(1, 0.25,0.25,0.95);
    glBegin(GL_LINES);
      glVertex2f( cts, depth);
      glVertex2f( cts, depth - chunk_height );

      glVertex2f( cts, depth - chunk_height );
      glVertex2f( cte, depth - chunk_height );

      glVertex2f( cte, depth - chunk_height );
      glVertex2f( cte, depth );

      glVertex2f( cte, depth );
      glVertex2f( cts, depth);
    glEnd();

  }
  draw_tags();
  vsx_widget::i_draw();
  profiler->sub_end();
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

void vsx_widget_profiler::event_mouse_down(vsx_widget_distance distance,vsx_widget_coords coords,int button)
{
  profiler->sub_begin("prof::event_mouse_down");

  double x = coords.world_global.x;
  double factor = world_to_time_factor(x);

  // decide which way to iterate
  long index = (long)(factor * consumer_chunks.size());


  selected_chunk = 0x0;

  int mouse_depth = depth_from_mouse_position();

  vsx_printf("mouse depth: %d\n", mouse_depth);

  if (mouse_depth < 0)
  {
    profiler->sub_end();
    return;
  }





  for (size_t i = 0; i < consumer_chunks.size(); i++)
  {
    if
    (
      factor < consumer_chunks[i].time_end * one_div_chunk_time_end
      &&
      factor > consumer_chunks[i].time_start * one_div_chunk_time_end
      &&
      mouse_depth == consumer_chunks[i].depth
    )
    {
      selected_chunk = &consumer_chunks[i];
      profiler->sub_end();
      return;
    }
  }

  profiler->sub_end();



//  if
//  (
//    factor < consumer_chunks[index].time_end * one_div_chunk_time_end
//    &&
//    factor > consumer_chunks[index].time_start * one_div_chunk_time_end
//    &&
//    mouse_depth == consumer_chunks[index].depth
//  )
//  {
//    selected_chunk = &consumer_chunks[index];
//  }

//  int traversion = -1;
//  if
//  (
//    factor > consumer_chunks[index].time_end * one_div_chunk_time_end
//    &&
//    factor > consumer_chunks[index].time_start * one_div_chunk_time_end
//  )
//  {
//    traversion = 1;
//  }

//  while (1)
//  {
//    index += traversion;

//    if (index > consumer_chunks.size() -1)
//      break;

//    if (index == -1)
//      break;

//    if
//    (
//      factor < consumer_chunks[index].time_end * one_div_chunk_time_end
//      &&
//      factor > consumer_chunks[index].time_start * one_div_chunk_time_end
//      &&
//      mouse_depth == consumer_chunks[index].depth
//    )
//    {
//      selected_chunk = &consumer_chunks[index];
//      break;
//    }
//  }
}


void vsx_widget_profiler::event_mouse_double_click(vsx_widget_distance distance,vsx_widget_coords coords,int button)
{
  VSX_UNUSED(distance);
  VSX_UNUSED(coords);
  VSX_UNUSED(button);
  if (!selected_chunk)
    return;

  double tdiff = selected_chunk->time_end - selected_chunk->time_start;
  vsx_printf("tdiff. %f\n", tdiff);
  time_scale = 0.5 / (tdiff) ;
  time_offset = -time_scale * (selected_chunk->time_start + 0.5 * tdiff);

  camera.set_pos( vsx_vector<>(0.0, 0.0, 1.9) );
  update_vbo();
  update_tag_draw_chunks();
}

void vsx_widget_profiler::event_mouse_move_passive(vsx_widget_distance distance,vsx_widget_coords coords)
{
  VSX_UNUSED(distance);
  mouse_pos = coords.world_global;
}


void vsx_widget_profiler::event_mouse_wheel(float y)
{
  if (ctrl)
  {
    vsx_printf("camera z: %f\n", camera.get_pos_z());

    time_offset -= 0.025 * y * (camera.get_pos_z() - 1.1);
    update_vbo();
    update_tag_draw_chunks();
    return;
  }

  float factor = world_to_time_factor( mouse_pos.x );

  float time_scale_pre = time_scale;

  time_scale *= 1.0 + chunk_height * y;

  time_offset -= factor * ( time_scale * chunk_time_end - time_scale_pre * chunk_time_end );

  update_vbo();
  update_tag_draw_chunks();

}

void vsx_widget_profiler::interpolate_size()
{
  vsx_widget::interpolate_size();
}
