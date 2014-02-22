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

#ifndef VSX_WIDGET_TIMELINE_H
#define VSX_WIDGET_TIMELINE_H

class vsx_widget_timeline : public vsx_widget {
  vsx_vector parentpos;
public:
  vsx_widget_sequence_editor* owner;

  void init() {
    auto_move_dir = 0;
  }

  float dd_time;
  float auto_move_dir;
  float a_dist;
  bool show_wave_data;
  void move_time(vsx_vector world);

  void event_mouse_down(vsx_widget_distance distance,vsx_widget_coords coords,int button)
  {
    VSX_UNUSED(coords);
    VSX_UNUSED(button);

    move_time(distance.center);
    a_focus = this;
  }

  void event_mouse_up(vsx_widget_distance distance,vsx_widget_coords coords,int button)
  {
    VSX_UNUSED(distance);
    VSX_UNUSED(coords);
    VSX_UNUSED(button);

    auto_move_dir = 0;
    owner->update_time_from_engine = true;
  }

  void event_mouse_move(vsx_widget_distance distance,vsx_widget_coords coords)
  {
    VSX_UNUSED(coords);
    move_time(distance.center);
  }

  float totalsize;
  float ff;
  float levelstart;

  void i_draw();
  void draw_waveform_data(float y_mid, float y_size_half);

  bool event_key_down(signed long key, bool alt, bool ctrl, bool shift);

  vsx_widget_timeline()
  {
    show_wave_data = false;
  }
};





#endif
