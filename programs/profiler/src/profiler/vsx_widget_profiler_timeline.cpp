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

#include "vsx_widget.h"
#include "vsx_widget_profiler_timeline.h"
#include <debug/vsx_error.h>
#include <gl_helper.h>

vsx_widget_profiler_timeline::vsx_widget_profiler_timeline()
  :
    dd_time(0.0),
    auto_move_dir(0.0),
    a_dist(0.0),
    time_holder(0x0)
{
}

void vsx_widget_profiler_timeline::time_holder_set(vsx_widget_time_holder* n)
{
  time_holder = n;
}

vsx_widget_time_holder* vsx_widget_profiler_timeline::time_holder_get()
{
  return time_holder;
}



void vsx_widget_profiler_timeline::move_time(vsx_vector2f world)
{
  if (!time_holder)
    VSX_ERROR_RETURN("time_holder not set");

  float f = (world.x+size.x/2)/(size.x);
  if (f < 0) f = 0;
  else
  if (f > 1) f = 1;
  float c_time = time_holder->start_get() + f * ( time_holder->end_get() - time_holder->start_get() );
  if (c_time < 0) c_time = 0;

  float a = (c_time - time_holder->start_get()) / time_holder->diff();
  if (a > 0.95) {
    auto_move_dir = 1;
    a_dist = a-0.95;
  } else
  if (a < 0.05) {
    auto_move_dir = -1;
    a_dist = 0.05-a;
  } else {
    auto_move_dir = 0;
  }
  time_holder->cur_set( c_time );
}

void vsx_widget_profiler_timeline::auto_move_time()
{
  if (!time_holder)
    VSX_ERROR_RETURN("time_holder not set");

  float time_diff = time_holder->diff();
  if (auto_move_dir) {
    float dtime = vsx_widget_time::get_instance()->get_dtime();
    float ncurtime = time_holder->cur_get() + auto_move_dir * dtime * time_diff*10.0f*a_dist;
    float ntstart = time_holder->start_get() + auto_move_dir * dtime*time_diff*10.0f*a_dist;
    float ntend = time_holder->end_get() + auto_move_dir*dtime*time_diff*10.0f*a_dist;
    if (time_holder->cur_get() >= 0)
    {
      time_holder->cur_set( ncurtime );
      time_holder->start_set( ntstart );
      time_holder->end_set( ntend );
    }
  }

}

void vsx_widget_profiler_timeline::i_draw()
{
  if (!time_holder)
    VSX_ERROR_RETURN("time_holder not set");

  auto_move_time();

  parentpos = parent->get_pos_p();
  float time_diff = time_holder->diff();
  float totalsize = time_holder->diff();

  float y_mid = parentpos.y+pos.y;
  float y_size = size.y;
  if (a_focus == this) y_size *= 3.0f;
  float y_size_half = y_size * 0.5f;

  float ff;


  glBegin(GL_QUADS);
    if (time_holder->start_get() < 0) {
      glColor4f(0.4,0.3,0.3f,0.5f);
      ff = size.x*(fabs(time_holder->start_get())/time_diff);
      glVertex2f(parentpos.x+pos.x-size.x*0.5f, y_mid+y_size_half);
      glVertex2f(parentpos.x+pos.x-size.x*0.5f+ff, y_mid+y_size_half);
      glVertex2f(parentpos.x+pos.x-size.x*0.5f+ff, y_mid-y_size_half);
      glVertex2f(parentpos.x+pos.x-size.x*0.5f, y_mid-y_size_half);
      if (time_holder->end_get() > 0) {
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

  draw_box_border(
    vsx_vector3<>( parentpos.x+pos.x - size.x * 0.5, y_mid-y_size_half ),
    vsx_vector3<>( size.x, y_size ), dragborder * 0.5
  );

  float levelstart = 0;

  font.color.a = 0.8f;

  float one_div_totalsize_times_sizex = 1.0f / totalsize * size.x;
  for (int i = (int)time_holder->start_get(); i < (int)(time_holder->end_get())+1; ++i)
  {
    glColor3f(0.5,0.5,0.5);
    float x =  (float)( i - time_holder->start_get()) * one_div_totalsize_times_sizex;
    if (x > 0)
    {
      x += parentpos.x+pos.x - size.x*0.5f+levelstart*size.x;
      glBegin(GL_LINES);
        glVertex2f(x,y_mid+y_size*0.416666667f);
        glVertex2f(x,y_mid-y_size*0.416666667f);
      glEnd();
      font.print_center(vsx_vector3<>(x,y_mid), vsx_string_helper::i2s(i),0.02);
    }
  }

  glColor3f(1,1,1);
  float f = ((time_holder->cur_get() - time_holder->start_get()) /
             time_holder->diff())
            * size.x;

  glBegin(GL_LINES);
    glVertex2f(parentpos.x+pos.x-size.x*0.5f+f,y_mid+y_size*0.416666667);
    glVertex2f(parentpos.x+pos.x-size.x*0.5f+f,y_mid-y_size*0.416666667);
  glEnd();

}

bool vsx_widget_profiler_timeline::event_key_down(uint16_t key)
{
  if (!time_holder)
    VSX_ERROR_RETURN_V("time_holder not set", true);

  if (!vsx_input_keyboard.pressed_ctrl())
    return true;

  float dt = time_holder->diff() * 0.5;

  switch(key) {
    case 'f':
        time_holder->start_inc(dt*0.03);
        time_holder->end_inc(dt*0.03);
        return false;
    break;

    case 's':
      time_holder->start_inc(-dt*0.03);
      time_holder->end_inc(-dt*0.03);
        return false;
    break;

    case 'w':
        if ( time_holder->cur_get() < time_holder->start_get() )
        {
          time_holder->start_set( time_holder->cur_get() );
          time_holder->end_set( time_holder->cur_get() + dt*2 );
        } else
        if (time_holder->cur_get() > time_holder->end_get())
        {
          time_holder->start_set( time_holder->cur_get()-dt*2);
          time_holder->end_set( time_holder->cur_get() );
        }
        time_holder->start_inc( -(time_holder->cur_get() - time_holder->start_get())/(dt)*dt*0.03 );
        time_holder->end_inc( (time_holder->end_get() - time_holder->cur_get())/(dt)*dt*0.03 );
        return false;
    break;

    case 'r':
      if (time_holder->cur_get() < time_holder->start_get() || time_holder->cur_get() > time_holder->end_get())
      {
        time_holder->start_set( time_holder->cur_get() - dt );
        time_holder->end_set( time_holder->cur_get() + dt );
      }

      time_holder->start_inc( (time_holder->cur_get() - time_holder->start_get())/(dt)*dt*0.03 );
      time_holder->end_inc( - (time_holder->end_get() - time_holder->cur_get())/(dt)*dt*0.03 );
      return false;
    break;
  }
  return true;

}
