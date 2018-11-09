/**
* Project: VSXu launcher - Data collection and data visualizer
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

#pragma once

// widget
#include <vsx_widget.h>
#include <widgets/vsx_widget_popup_menu.h>
#include <widgets/vsx_widget_editor.h>
#include <widgets/vsx_widget_base_edit.h>
#include <widgets/vsx_widget_label.h>
#include <widgets/vsx_widget_dropbox.h>
#include <widgets/vsx_widget_checkbox.h>
#include <vsx_application_display.h>
#include <vsx_argvector.h>
#include <tools/vsx_process.h>
#include <container/vsx_nw_vector.h>
#include <RtAudio/RtAudio.h>

// engine_graphics
#include <gl_helper.h>
#include <vsx_vbo_bucket.h>

using std::unique_ptr;

class vsx_widget_launcher : public vsx_widget
{
  RtAudio* rtaudio = 0x0;
  int target_audio_device = -1;

public:

  vsx_widget_label* label = 0x0;

  vsx_widget_label* application_label = 0x0;
  vsx_widget_dropbox* application_selection = 0x0;
  vsx_widget_checkbox* fullscreen = 0x0;

  vsx_widget_label* sound_input_label = 0x0;
  vsx_widget_dropbox* sound_input_selection = 0x0;

  vsx_widget_label* display_label = 0x0;
  vsx_widget_dropbox* display_selection = 0x0;


  vsx_widget_label* resolution_label = 0x0;
  vsx_widget_base_edit* resolution_x_edit = 0x0;
  vsx_widget_label* resolution_label_mid = 0x0;
  vsx_widget_base_edit* resolution_y_edit = 0x0;

  vsx_widget_label* position_label = 0x0;
  vsx_widget_base_edit* position_x_edit = 0x0;
  vsx_widget_label* position_label_mid = 0x0;
  vsx_widget_base_edit* position_y_edit = 0x0;

  vsx_widget_checkbox* borderless = 0x0;

#if PLATFORM_FAMILY == PLATFORM_FAMILY_WINDOWS
  vsx_widget_checkbox* console = 0x0;
#endif

  vsx_widget_button* launch_button = 0x0;
  void set_window_position_to_center(size_t value);
  void init();
  void launch();
  void i_draw();
  void command_process_back_queue(vsx_command_s *t);
  bool event_key_down(uint16_t key);
  void event_mouse_wheel(float y);
  void event_mouse_move_passive(vsx_widget_distance distance,vsx_widget_coords coords);
  void interpolate_size();

};

