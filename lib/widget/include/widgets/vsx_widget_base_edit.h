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

#ifndef VSX_WIDGET_BASE_EDIT_H
#define VSX_WIDGET_BASE_EDIT_H

#include "vsx_widget_dllimport.h"
#include "vsx_widget_panel.h"

/* Base Edit
 * ---------
 * This widget serves as a text editor and item picker. If you turn editing off
 * it can be used as a selection box.
 *
 */

class WIDGET_DLLIMPORT vsx_widget_base_edit : public vsx_widget_panel {
  vsx_nw_vector <vsx_string<> > lines;
  std::vector <vsx_string<> > lines_p;
  std::vector<int> lines_visible;
  std::vector<int> lines_visible_stack;
  std::vector<vsx_widget*> action_buttons;
  // TODO: delete the action buttons in the destructor
  float longest_line;
  float longest_y;
  int num_hidden_lines;
  vsx_string<>syntax_col[5];
  bool process_characters;
  void process_line(int n_line);
  void process_lines();
  void calculate_scroll_size();
  void render_caret();
  bool filter_string_enabled;

public:
  int caretx, carety;
	int updates;
	bool enable_syntax_highlighting;
	bool selected_line_highlight;
	bool enable_line_action_buttons;
  bool draw_line_numbers = false;
	int selected_line;
  vsx_string<>command_prefix; // sent to parent in command when single row (command hog)
  vsx_string<>allowed_chars; // if empty all are allowed
  bool editing_enabled; // can the text be edited?
  bool single_row; // is single row editor only?
  std::map<vsx_string<>,char> keywords;


  int inside_xy_l(vsx_vector3<> &test, vsx_vector3<> &global) { return vsx_widget::inside_xy_l(test,global); }
  void command_process_back_queue(vsx_command_s *t);
  vsx_widget* mirror_keystrokes_object; // vsx_widget that gets a key event when the string has been modified (not on arrow keys), default: 0
  vsx_widget* mirror_mouse_move_passive_object;
  vsx_widget* mirror_mouse_move_object;
  vsx_widget* mirror_mouse_down_object;
  vsx_widget* mirror_mouse_up_object;
  vsx_widget* mirror_mouse_double_click_object;
  float scrollbar_pos_x, scrollbar_pos_y;
  float scroll_x, scroll_y;
  float scroll_x_size, scroll_y_size;
  float scroll_x_max, scroll_y_max;
  float characters_width, characters_height;
  void set_string(const vsx_string<>& str);
  void caret_goto_end();
  vsx_string<> get_string();
  vsx_string<> get_line(unsigned long line);

  void i_draw();
  virtual void event_mouse_down(vsx_widget_distance distance,vsx_widget_coords coords,int button);
  void event_mouse_up(vsx_widget_distance distance,vsx_widget_coords coords,int button);
  void event_mouse_move(vsx_widget_distance distance,vsx_widget_coords coords);
  void event_mouse_move_passive(vsx_widget_distance distance,vsx_widget_coords coords);
  void event_text(wchar_t character_wide, char character);
  bool event_key_down(uint16_t key);
  virtual void event_mouse_double_click(vsx_widget_distance distance,vsx_widget_coords coords,int button);
  void event_mouse_wheel(float y);
  void set_filter_string(vsx_string<>&filter_string);
  void fold_all();
  vsx_widget_base_edit();

};

#endif
