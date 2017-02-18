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

#include "vsx_widget_controller_channel.h"
#include "vsx_widget_controller_knob.h"
#include "vsx_widget_controller_slider.h"

vsx_widget_controller_channel::vsx_widget_controller_channel()
{
  generate_menu();
  menu->init();
  target_size.y = size.y=sizeunit*6.75;
}

void vsx_widget_controller_channel::init()
{
  if (init_run) return;
  vsx_widget_controller_base::init();
  get_value();
  amp = add(new vsx_widget_controller_knob,"amp");
  amp->set_pos(vsx_vector3<>(0,pos.y+size.y/2-amp->size.y*0.75));

  ((vsx_widget_controller_knob*)amp)->bgcolor.a=0;
  ((vsx_widget_controller_knob*)amp)->owned=true;
  ((vsx_widget_controller_knob*)amp)->drawconnection=false;
  ((vsx_widget_controller_knob*)amp)->isolate=true;
  ((vsx_widget_controller_knob*)amp)->init();
  ((vsx_widget_controller_knob*)amp)->capmin = true;
  ((vsx_widget_controller_knob*)amp)->capminv = ALMOST_ZERO;

  offset = add(new vsx_widget_controller_knob,"offset");
  offset->set_pos(vsx_vector3<>(0,amp->pos.y-offset->size.y-offset->size.y*0.25));
  ((vsx_widget_controller_knob*)offset)->bgcolor.a=0;
  ((vsx_widget_controller_knob*)offset)->owned=true;
  ((vsx_widget_controller_knob*)offset)->drawconnection=false;
  ((vsx_widget_controller_knob*)offset)->isolate=true;
  ((vsx_widget_controller_knob*)offset)->init();

  slider = add(new vsx_widget_controller_slider,"slider");
  slider->set_pos(vsx_vector3<>(0,offset->pos.y-(offset->size.y/2)-(slider->size.y/2)-(sizeunit/4)));
  ((vsx_widget_controller_slider*)slider)->bgcolor.a=0;
  ((vsx_widget_controller_slider*)slider)->owned=true;
  ((vsx_widget_controller_slider*)slider)->drawconnection=false;
  ((vsx_widget_controller_slider*)slider)->isolate=true;
  ((vsx_widget_controller_slider*)slider)->value=value;
  ((vsx_widget_controller_slider*)slider)->init();
  ((vsx_widget_controller_slider*)slider)->capmin = capmin;
  ((vsx_widget_controller_slider*)slider)->capmax = capmax;
  ((vsx_widget_controller_slider*)slider)->capminv = capminv;
  ((vsx_widget_controller_slider*)slider)->capmaxv = capmaxv;

  double x = ((vsx_widget_controller_slider*)slider)->amp_get();
  double i = ((vsx_widget_controller_slider*)slider)->ofs_get();

  ((vsx_widget_controller_knob*)offset)->set_value(i);
  ((vsx_widget_controller_knob*)amp)->set_value(x-i);
  smooth(smoothness);

  if (!isolate) command_q_b.add_raw("param_get "+target_param + " " + vsx_string_helper::i2s(slider->id));
}

void vsx_widget_controller_channel::smooth(float newval)
{
  smoothness=newval;
  if (init_run) {
    ((vsx_widget_controller_knob*)amp)->smooth(newval);
    ((vsx_widget_controller_knob*)offset)->smooth(newval);
    ((vsx_widget_controller_knob*)slider)->smooth(newval);
  }
}

void vsx_widget_controller_channel::command_process_back_queue(vsx_command_s *t)
{
  if (t->cmd=="pg64_ok" && !isolate) {
    set_value(vsx_string_helper::s2f(vsx_string_helper::base64_decode(t->parts[3])));
    if (target_value > ((vsx_widget_controller_slider*)slider)->amp_get())
    {
      ((vsx_widget_controller_knob*)amp)->target_value = target_value*2;
      ((vsx_widget_controller_slider*)slider)->amp_set( target_value*2 );
    }
    if (target_value < ((vsx_widget_controller_slider*)slider)->ofs_get())
    {
      ((vsx_widget_controller_knob*)offset)->value = target_value*2;
      ((vsx_widget_controller_knob*)offset)->target_value = target_value*2;
      ((vsx_widget_controller_slider*)slider)->ofs_set( target_value*2 );

      ((vsx_widget_controller_knob*)amp)->value = ((vsx_widget_controller_knob*)amp)->target_value - target_value*2;
      ((vsx_widget_controller_knob*)amp)->target_value = ((vsx_widget_controller_knob*)amp)->target_value - target_value*2;
      ((vsx_widget_controller_slider*)slider)->amp_set( ((vsx_widget_controller_knob*)amp)->target_value - target_value*2 );
    }
    ((vsx_widget_controller_slider*)slider)->set_value(value);
  } else
  if (t->cmd == "set_value_i")
  {
    if (value > ((vsx_widget_controller_slider*)slider)->amp_get())
    {
      ((vsx_widget_controller_knob*)amp)->target_value = target_value*2;
      ((vsx_widget_controller_slider*)slider)->amp_set( target_value*2 );
    }

    if (value < ((vsx_widget_controller_slider*)slider)->ofs_get())
    {
      ((vsx_widget_controller_knob*)offset)->value = target_value*2;
      ((vsx_widget_controller_knob*)offset)->target_value =  target_value*2;
      ((vsx_widget_controller_slider*)slider)->ofs_set( target_value*2 );

      ((vsx_widget_controller_knob*)amp)->value = ((vsx_widget_controller_knob*)amp)->target_value-target_value*2;
      ((vsx_widget_controller_knob*)amp)->target_value =  ((vsx_widget_controller_knob*)amp)->target_value-target_value*2;
      ((vsx_widget_controller_slider*)slider)->amp_set( ((vsx_widget_controller_knob*)amp)->target_value-target_value*2 );
    }
    ((vsx_widget_controller_slider*)slider)->set_value(value);
  } else
  if (t->cmd == "update") {
    if (t->parts[1] == "amp") {
      ((vsx_widget_controller_slider*)slider)->amp_set( ((vsx_widget_controller_knob*)amp)->send_value );
      ((vsx_widget_controller_slider*)slider)->ofs_set( ((vsx_widget_controller_knob*)offset)->send_value );
      ((vsx_widget_controller_slider*)slider)->cap_value();
      ((vsx_widget_controller_slider*)slider)->value = ((vsx_widget_controller_slider*)slider)->target_value;
      if (((vsx_widget_controller_slider*)slider)->value != ((vsx_widget_controller_slider*)slider)->target_value)
      {
        set_value(((vsx_widget_controller_slider*)slider)->target_value);
        if (((vsx_widget_controller_knob*)amp)->smoothness>=0) command_q_b.add_raw("param_set_interpolate "+ vsx_string_helper::f2s(((vsx_widget_controller_slider*)slider)->target_value)  + " " + vsx_string_helper::f2s(((vsx_widget_controller_knob*)amp)->smoothness) + " "+target_param);
        else command_q_b.add_raw("param_set "+ vsx_string_helper::f2s(((vsx_widget_controller_slider*)slider)->target_value) + " "+target_param);
        parent->vsx_command_queue_b(this);
      }
    } else
    if (t->parts[1] == "offset") {
      ((vsx_widget_controller_slider*)slider)->amp_set( ((vsx_widget_controller_knob*)amp)->send_value );
      ((vsx_widget_controller_slider*)slider)->ofs_set( ((vsx_widget_controller_knob*)offset)->send_value );
      ((vsx_widget_controller_slider*)slider)->cap_value();
      ((vsx_widget_controller_knob*)offset)->target_value = ((vsx_widget_controller_slider*)slider)->ofs_get();
      ((vsx_widget_controller_slider*)slider)->value = ((vsx_widget_controller_slider*)slider)->target_value;
      if (value != ((vsx_widget_controller_slider*)slider)->target_value)
      {
        //target_value = value = ((vsx_widget_slider*)slider)->value;
        set_value(((vsx_widget_controller_slider*)slider)->target_value);
        if (((vsx_widget_controller_knob*)offset)->smoothness>=0) command_q_b.add_raw("param_set_interpolate "+ vsx_string_helper::f2s(((vsx_widget_controller_slider*)slider)->target_value)  + " " + vsx_string_helper::f2s(((vsx_widget_controller_knob*)offset)->smoothness) + " "+target_param);
        else command_q_b.add_raw("param_set "+ vsx_string_helper::f2s(((vsx_widget_controller_slider*)slider)->target_value) + " "+target_param);
        parent->vsx_command_queue_b(this);
      }
    } else
    if (t->parts[1] == "slider") {
      //printf("slider update\n");
      //printf("smoothing: %f\n",smoothness);
      if (value!=((vsx_widget_controller_slider*)slider)->target_value)
      {
        //target_value = value = ((vsx_widget_slider*)slider)->value;
        //printf("set value: %f\n",((vsx_widget_slider*)slider)->target_value);
        set_value(((vsx_widget_controller_slider*)slider)->target_value);
        smoothness = ((vsx_widget_controller_slider*)slider)->smoothness;
        if (smoothness>=0) command_q_b.add_raw("param_set_interpolate "+ vsx_string_helper::f2s(((vsx_widget_controller_slider*)slider)->target_value)  + " " + vsx_string_helper::f2s(smoothness) + " "+target_param);
        else command_q_b.add_raw("param_set "+ vsx_string_helper::f2s(((vsx_widget_controller_slider*)slider)->target_value) + " "+target_param);
        parent->vsx_command_queue_b(this);
      }
    }
    ((vsx_widget_controller_slider*)slider)->cap_value();
    return;
  } else
  vsx_widget_controller_base::command_process_back_queue(t);
}

void vsx_widget_controller_channel::draw()
{
  if (!visible) return;
  vsx_widget_controller_base::draw();
  draw_children();
}
