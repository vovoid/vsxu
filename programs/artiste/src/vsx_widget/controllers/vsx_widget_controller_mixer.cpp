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


#include "vsx_widget_controller_mixer.h"
#include "vsx_widget_controller_channel.h"
#include "vsx_widget_controller_slider.h"


vsx_widget_controller_mixer::vsx_widget_controller_mixer()
{
  generate_menu();
  menu->init();
  nummixers=4;
}

void vsx_widget_controller_mixer::init()
{
  if (init_run) return;
  vsx_widget_controller_base::init();
  set_size(vsx_vector3<>(sizeunit*nummixers,sizeunit*6.75));
  target_pos.x = (pos.x -= size.x*0.5);

  for (int t=0;t<nummixers;++t)
  {
    vsx_widget* newmixer=add(new vsx_widget_controller_channel,name+".channel"+vsx_string_helper::i2s(nummixers-t));
    newmixer->set_pos(vsx_vector3<>(-size.x*0.5+sizeunit*0.5 + (float)t*sizeunit));
    //newmixer->pos.x = -size.x*0.5+sizeunit*0.5;
    //newmixer->pos.x+=(float)t*sizeunit;
    ((vsx_widget_controller_channel*)newmixer)->owned=true;
    ((vsx_widget_controller_channel*)newmixer)->drawconnection=false;
    ((vsx_widget_controller_channel*)newmixer)->isolate=true;
    ((vsx_widget_controller_channel*)newmixer)->init();
    //((vsx_widget_3d_hint*)((vsx_widget_slider*)((vsx_widget_channel*)newmixer)->slider)->hint)->title=target_param+'['+vsx_string_helper::i2s(t)+']';
    mixers.push_back(newmixer);
  }
  vsx_nw_vector <vsx_string<> > parts;
  vsx_string<>deli = ";";
  vsx_string_helper::explode(capmaxv_s,deli,parts);
  for (unsigned int i = 0; i < parts.size(); ++i) {
    if (parts[i] != "x" && parts[i] != "") {
      ((vsx_widget_controller_slider*)((vsx_widget_controller_channel*)mixers[i])->slider)->capmax = true;
      ((vsx_widget_controller_slider*)((vsx_widget_controller_channel*)mixers[i])->slider)->capmaxv = vsx_string_helper::s2f(parts[i]);
    }
  }
  parts.clear();
  vsx_string_helper::explode(capminv_s,deli,parts);
  for (unsigned int i = 0; i < parts.size(); ++i) {
    if (parts[i] != "x" && parts[i] != "") {
      ((vsx_widget_controller_slider*)((vsx_widget_controller_channel*)mixers[i])->slider)->capmin = true;
      ((vsx_widget_controller_slider*)((vsx_widget_controller_channel*)mixers[i])->slider)->capminv = vsx_string_helper::s2f(parts[i]);
    }
  }
  smooth(smoothness);
  get_value();
}

void vsx_widget_controller_mixer::smooth(float newval)
{
  for (unsigned long i=0;i<mixers.size(); ++i)
    ((vsx_widget_controller_channel*)mixers[i])->smooth(newval);
}

void vsx_widget_controller_mixer::get_in_param_spec(std::pair<vsx_string<>,vsx_string<> > parampair)
{
  //printf("grombleton\n");
  if (parampair.first=="max") { capmaxv_s=parampair.second; capmax=true; } else
  if (parampair.first=="min") { capminv_s=parampair.second; capmin=true; } else
  vsx_widget_controller_base::get_in_param_spec(parampair);
}


void vsx_widget_controller_mixer::draw()
{
  if (!visible) return;
  vsx_widget_controller_base::draw();
  //draw_children();
}

void vsx_widget_controller_mixer::command_process_back_queue(vsx_command_s *t)
{
  if (t->cmd == "pg64_ok") {
    //printf("param_get_ok\n");
    vsx_nw_vector <vsx_string<> > parts;
    vsx_string<>deli = ",";
    t->parts[3] = vsx_string_helper::base64_decode(t->parts[3]);
    vsx_string_helper::explode(t->parts[3],deli, parts);
    for (unsigned long i = 0; i < parts.size(); ++i)
    {
      ((vsx_widget_controller_channel*)mixers[i])->set_value(vsx_string_helper::s2f(parts[i]));
      command_q_b.add_raw("set_value_i");
      ((vsx_widget_controller_channel*)mixers[i])->vsx_command_queue_b(this);
    }
  } else
  if (t->cmd == "param_set_interpolate" || t->cmd == "param_set")
  {

    vsx_string<>cmd="";
    for (unsigned long i=0;i<mixers.size();++i)
    {
      if (vsx_input_keyboard.pressed_ctrl() && vsx_input_keyboard.pressed_shift())
      {
        cmd += t->parts[1];
        ((vsx_widget_controller_channel*)mixers[i])->set_value(vsx_string_helper::s2f(t->parts[1]));
        command_q_b.add_raw("set_value_i");
        ((vsx_widget_controller_channel*)mixers[i])->vsx_command_queue_b(this);

        //((vsx_widget_controller_channel*)mixers[i])->smooth(vsx_string_helper::s2f( t->parts[1] ));
      } else
      {
        cmd += vsx_string_helper::f2s( ( (vsx_widget_controller_channel*)mixers[i])->target_value );
      }
      if ( i < mixers.size()-1 ) cmd += ",";
    }
    command_q_b.add_raw(t->cmd + " " +cmd+ " " + t->parts[t->parts.size()-1]);
    parent->vsx_command_queue_b(this);
    return;
  } else
  vsx_widget_controller_base::command_process_back_queue(t);
}
