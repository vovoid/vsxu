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

#ifndef VSX_WIDGET_CONTROLLER_SEQ_H
#define VSX_WIDGET_CONTROLLER_SEQ_H

#include "vsx_widget_controller_base.h"

class vsx_widget_controller_sequence : public vsx_widget_controller_base
{
private:
  vsx_widget* seq_chan;
  int size_controlled_from_outside;
  int draw_base;
  int parent_removal;
public:
  void init();
  virtual void command_process_back_queue(vsx_command_s *t);
  void set_size_controlled_from_outside(int new_value) { size_controlled_from_outside = new_value; }
  void set_span(float, float);
  void set_view_time(float, float);
  void set_draw_base(int new_val)
  {
  	draw_base = new_val;
  }
  void set_parent_removal(int new_val)
  {
  	parent_removal = new_val;
  }
  virtual void set_size(vsx_vector3<> new_size);

  void set_param_type(int n);

  virtual void i_draw();
  vsx_widget_controller_sequence() :
  		seq_chan(0x0),
  		size_controlled_from_outside(0),
  		draw_base(1),
  		parent_removal(0)
  {}
};




#endif

