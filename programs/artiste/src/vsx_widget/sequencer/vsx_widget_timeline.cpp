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
#include <command/vsx_command.h>
#include <command/vsx_command_list.h>
#include <command/vsx_command_client_server.h>
#include "vsx_font.h"
#include <filesystem/vsx_filesystem.h>
#include "vsx_param.h"
#include <module/vsx_module.h>

// local includes
#include "vsx_widget.h"
#include "vsx_widget_sequence_editor.h"
#include "vsx_widget_sequence_channel.h"
#include "vsx_widget_timeline.h"
#include "server/vsx_widget_server.h"

// engine includes
#include "vsx_engine.h"
#include <gl_helper.h>


void vsx_widget_timeline::move_time(vsx_vector2f world)
{
  if (owner->engine_status == VSX_ENGINE_STOPPED)
  {
    float c_time = time_from_click_coordinate(world);
    if (vsx_input_keyboard.pressed_ctrl())
    {
      owner->time_selection_right = c_time;
      owner->time_selection_active = true;
      if (owner->time_selection_right < owner->time_selection_left)
        owner->time_selection_active = false;
      return;
    }

    float a = (c_time - owner->time_left_border)/(owner->time_right_border - owner->time_left_border);

    if (a > 0.95) {
      auto_move_dir = 1;
      owner->update_time_from_engine = false;
      a_dist = a-0.95;
    } else
    if (a < 0.05) {
      auto_move_dir = -1;
      a_dist = 0.05-a;
      owner->update_time_from_engine = false;
    } else {
      auto_move_dir = 0;
      owner->time = c_time;
      owner->time_selection_active = false;
      owner->time_selection_left = owner->time;

      command_q_b.add_raw("time_set "+vsx_string_helper::f2s(owner->time));
      parent->vsx_command_queue_b(this);
      owner->update_time_from_engine = false;
    }
  } else {
    auto_move_dir = 0;
    owner->update_time_from_engine = true;
  }
}

void vsx_widget_timeline::i_draw()
{
  parentpos = parent->get_pos_p();
  float time_diff = owner->time_right_border - owner->time_left_border;
  totalsize = (owner->time_right_border - owner->time_left_border);

  float y_mid = parentpos.y + pos.y;
  float y_size = size.y;
  if (a_focus == this) y_size *= 3.0f;
  float y_size_half = y_size * 0.5f;

  if (auto_move_dir) {
    float dtime = vsx_widget_time::get_instance()->get_dtime();
    float curtime = owner->time + auto_move_dir * dtime * time_diff*10.0f*a_dist;
    float tstart = owner->time_left_border + auto_move_dir*dtime*time_diff*10.0f*a_dist;
    float tend = owner->time_right_border + auto_move_dir*dtime*time_diff*10.0f*a_dist;
    if (curtime >= 0) {
      owner->time = curtime;
      owner->time_left_border = tstart;
      owner->time_right_border = tend;
    }
    command_q_b.add_raw("time_set "+vsx_string_helper::f2s(owner->time));
    parent->vsx_command_queue_b(this);
  }

  glBegin(GL_QUADS);
    if (owner->time_left_border < 0) {
      glColor4f(0.4,0.3,0.3f,0.5f);
      ff = size.x*(fabs(owner->time_left_border)/totalsize);
      glVertex2f(parentpos.x+pos.x-size.x*0.5f, y_mid+y_size_half);
      glVertex2f(parentpos.x+pos.x-size.x*0.5f+ff, y_mid+y_size_half);
      glVertex2f(parentpos.x+pos.x-size.x*0.5f+ff, y_mid-y_size_half);
      glVertex2f(parentpos.x+pos.x-size.x*0.5f, y_mid-y_size_half);
      if (owner->time_right_border > 0) {
        glColor4f(0.3,0.4,0.3,0.5f);
        glVertex2f(parentpos.x+pos.x-size.x*0.5f+ff, y_mid+y_size_half);
        glVertex2f(parentpos.x+pos.x+size.x*0.5f, y_mid+y_size_half);
        glVertex2f(parentpos.x+pos.x+size.x*0.5f, y_mid-y_size_half);
        glVertex2f(parentpos.x+pos.x-size.x*0.5f+ff, y_mid-y_size_half);
      }
    } else {
      glColor4f(0.3,0.4,0.3,0.5f);
      glVertex2f(parentpos.x+pos.x-size.x*0.5f, y_mid+y_size_half);
      glVertex2f(parentpos.x+pos.x+size.x*0.5f, y_mid+y_size_half);
      glVertex2f(parentpos.x+pos.x+size.x*0.5f, y_mid-y_size_half);
      glVertex2f(parentpos.x+pos.x-size.x*0.5f, y_mid-y_size_half);
    }
  glEnd();

  if (a_focus == this)
    glColor3f(1.0f,1.0f,1.0f);
  else
    glColor3f(0.5f,0.5f,0.5f);

  draw_box_border(vsx_vector3<>(parentpos.x+pos.x-size.x*0.5,y_mid-y_size_half), vsx_vector3<>(size.x,y_size), dragborder*0.5);

  levelstart = ((float)(int)(trunc(owner->time_left_border+(owner->time_left_border>=0.0?0.5:-0.5))) - owner->time_left_border)/totalsize;

  levelstart = 0;

  font.color.a = 0.8f;

  // vertical markings per second
  float one_div_totalsize_times_sizex = 1.0f / totalsize * size.x;
  for (int i = (int)owner->time_left_border; i < (int)(owner->time_right_border)+1; ++i)
  {
    glColor3f(0.5,0.5,0.5);
    float x =  (float)(i-owner->time_left_border) * one_div_totalsize_times_sizex;
    if (x > 0)
    {
      x += parentpos.x+pos.x - size.x*0.5f+levelstart*size.x;
      glBegin(GL_LINES);
        glVertex2f(x,y_mid+y_size*0.416666667f);
        glVertex2f(x,y_mid-y_size*0.416666667f);
      glEnd();
      font.print_center(vsx_vector3<>(x,y_mid), vsx_string_helper::i2s(i),0.005);
    }
  }

  float y_top = y_mid+y_size*0.416666667;
  float y_bottom = y_mid-y_size*0.416666667;

  // current selection area
  if (owner->time_selection_active)
  {
    float left_selection_factor = ((owner->time_selection_left - owner->time_left_border) /
                                 (owner->time_right_border - owner->time_left_border))
                                * size.x
        + parentpos.x+pos.x-size.x * 0.5f;
    float right_selection_factor = ((owner->time_selection_right - owner->time_left_border) /
                                 (owner->time_right_border - owner->time_left_border))
                                * size.x
        + parentpos.x+pos.x-size.x * 0.5f;
    glColor4f(0.7, 0.7, 1.0, 0.2);
    glBegin(GL_QUADS);
      glVertex2f( left_selection_factor, y_top );
      glVertex2f( right_selection_factor, y_top );
      glVertex2f( right_selection_factor, y_bottom);
      glVertex2f( left_selection_factor, y_bottom);
    glEnd();
  }


  // line denoting current time
  glColor3f(1,1,1);
  float current_time_factor = ((owner->time - owner->time_left_border) /
             (owner->time_right_border - owner->time_left_border))
            * size.x;

  glBegin(GL_LINES);
    glVertex2f(parentpos.x+pos.x-size.x*0.5f+current_time_factor, y_top);
    glVertex2f(parentpos.x+pos.x-size.x*0.5f+current_time_factor, y_bottom);
  glEnd();

  draw_waveform_data( y_mid, y_size_half );
}


void vsx_widget_timeline::draw_waveform_data(float y_mid, float y_size_half)
{
  req(show_wave_data);
  req(!owner->time_selection_active);

  vsx_widget_server* server = (vsx_widget_server*)owner->get_server();
  vsx_engine* engine = (vsx_engine*)(server->engine);
  vsx_module_engine_state* engine_info = engine->get_engine_info();

  if (engine_info->param_float_arrays.size() >= 4 && a_focus == this)
  {
    vsx_module_engine_float_array *full_pcm_data_l;
    vsx_module_engine_float_array *full_pcm_data_r;
    full_pcm_data_l = engine_info->param_float_arrays[2];
    full_pcm_data_r = engine_info->param_float_arrays[3];
    if (full_pcm_data_l->array.size() > 0)
    {
      // assuming we have 44100 samples per second
      float x_start = parentpos.x+pos.x-size.x*0.5f;
      float x_end   = parentpos.x+pos.x+size.x*0.5f;
      float t_start = owner->time_left_border;
      float t_end = owner->time_right_border;
      size_t data_end = (size_t) (t_end * 44100.0f);
      if (owner->time_left_border < 0)
      {
        x_start += fabs(t_start / (t_end - t_start)) * size.x;
        //data_end -= fabs(t_start) * 44100.0f;
        t_start = 0.0f;
      }
      size_t data_start = (size_t) (t_start * 44100.0f);

      size_t data_count = data_end - data_start;
      float x_dist = x_end - x_start;
      double x_diff = (double)x_dist / (double)data_count;

      glColor4f(1.0f,0.2f,0.2f,0.15f);
      double x_pos = x_start;
      glBegin(GL_LINE_STRIP);
        for (
              size_t i = data_start;
              i < data_end;
              i++
            )
        {
          glVertex2f(x_pos, y_mid + (*full_pcm_data_l).array[i] * y_size_half );
          x_pos += x_diff;
        }
      glEnd();


      glColor4f(0.2f,1.0f,0.2f,0.1f);
      x_pos = x_start;
      glBegin(GL_LINE_STRIP);
        for (
              size_t i = data_start;
              i < data_end;
              i++
            )
        {
          glVertex2f(x_pos, y_mid + (*full_pcm_data_r).array[i] * y_size_half );
          x_pos += x_diff;
        }
      glEnd();
    }
  }

}

bool vsx_widget_timeline::event_key_down(uint16_t key)
{
  float dt = (owner->time_right_border-owner->time_left_border)*0.5;
  switch(key) {
    case VSX_SCANCODE_A:
    {
      if (vsx_input_keyboard.pressed_ctrl())
        backwards_message("play");
      else
        backwards_message("stop");
    }
  }

  if (key == VSX_SCANCODE_T)
    show_wave_data = !show_wave_data;

  if (!vsx_input_keyboard.pressed_ctrl())
    return true;

  switch(key) {
    case VSX_SCANCODE_F:
        owner->time_left_border += dt*0.03;
        owner->time_right_border += dt*0.03;
        return false;
    break;

    case VSX_SCANCODE_S:
        owner->time_left_border -= dt*0.03;
        owner->time_right_border -= dt*0.03;
        return false;
    break;

    case VSX_SCANCODE_W:
        if (owner->time < owner->time_left_border) {
          owner->time_left_border = owner->time;
          owner->time_right_border = owner->time + dt*2;
        } else
        if (owner->time > owner->time_right_border) {
          owner->time_left_border = owner->time-dt*2;
          owner->time_right_border = owner->time;
        }
        owner->time_left_border -= (owner->time-owner->time_left_border)/(dt)*dt*0.03;
        owner->time_right_border += (owner->time_right_border-owner->time)/(dt)*dt*0.03;
        return false;
    break;

    case VSX_SCANCODE_R:
      if (owner->time < owner->time_left_border || owner->time > owner->time_right_border) {
        owner->time_left_border = owner->time - dt;
        owner->time_right_border = owner->time + dt;
      }

      owner->time_left_border += (owner->time-owner->time_left_border)/(dt)*dt*0.03;
      owner->time_right_border -= (owner->time_right_border-owner->time)/(dt)*dt*0.03;
      return false;
    break;
  }
  return true;
}
