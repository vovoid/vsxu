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

#include "vsx_widget_controller_dialog.h"
#include "vsx_widget_controller_knob.h"
#include "vsx_widget_controller_slider.h"

void vsx_widget_controller_dialog::draw() //include first, don't forget to add draw_children(); to the end of inheriting method
{
  vsx_widget_controller_base::draw();
  draw_children();
}

void vsx_widget_controller_dialog::init()
{
  if (init_run) return;
  vsx_widget_controller_base::init();
  init_children();
}

void vsx_widget_controller_dialog::get_in_param_spec(std::pair<vsx_string<>,vsx_string<> > parampair)
{
    vsx_widget_controller_base::get_in_param_spec(parampair);
    if (parampair.first.substr(0,8)=="control_")
    {
      vsx_widget* new_widget = 0x0;
      vsx_string<>type=parampair.first.substr(8);

      if (type=="knob")
        new_widget = add(new vsx_widget_controller_knob,name+".knob");

      if (type=="slider")
        new_widget = add(new vsx_widget_controller_slider,name+".slider");

      req(new_widget);

      ((vsx_widget_controller_base*)new_widget)->owned=true;
      ((vsx_widget_controller_base*)new_widget)->drawconnection=false;
      ((vsx_widget_controller_base*)new_widget)->in_param_spec=parampair.second;
      ((vsx_widget_controller_base*)new_widget)->init();
    }
}
