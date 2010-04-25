#ifndef VSX_WIDGET_BASE_EDIT_H
#define VSX_WIDGET_BASE_EDIT_H

#include "lib/vsx_widget_scrollbar.h"
#ifndef VSX_WIDGET_PANEL_H
#include "lib/vsx_widget_panel.h"
#endif
/* Base Edit
 * ---------
 * This widget serves as a text editor and item picker. If you turn editing off
 * it can be used as a selection box.
 *
 *
 *
 */

class vsx_widget_base_edit : public vsx_widget_panel {
  std::vector<vsx_string> lines;
  std::vector<vsx_string> lines_p;
  std::vector<int> lines_visible;
  std::vector<int> lines_visible_stack;
  std::vector<vsx_widget*> action_buttons;
  // TODO: delete the action buttons in the destructor
  float longest_line;
  float longest_y;
  int num_hidden_lines;
//  vsx_font myf;
  vsx_string syntax_col[5];
  bool process_characters;
  void process_line(int n_line);
  void process_lines();
  void calculate_scroll_size();
  bool filter_string_enabled;
public:
  int caretx, carety;
	int updates;
	bool enable_syntax_highlighting;
	bool selected_line_highlight;
	bool enable_line_action_buttons;
	int selected_line;
  vsx_string command_prefix; // sent to parent in command when single row (command hog)
  vsx_string allowed_chars; // if empty all are allowed
  bool editing_enabled; // can the text be edited?
  bool single_row; // is single row editor only?
  std::map<vsx_string,char> keywords;
  vsx_widget_base_edit();
  int inside_xy_l(vsx_vector &test, vsx_vector &global) { return vsx_widget::inside_xy_l(test,global); }
  void vsx_command_process_b(vsx_command_s *t);
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
  void set_string(const vsx_string& str);
  void caret_goto_end();
  vsx_string get_string();
  vsx_string get_line(unsigned long line);

  void i_draw();
  virtual void event_mouse_down(vsx_widget_distance distance,vsx_widget_coords coords,int button);
  void event_mouse_up(vsx_widget_distance distance,vsx_widget_coords coords,int button);
  void event_mouse_move(vsx_widget_distance distance,vsx_widget_coords coords);
  void event_mouse_move_passive(vsx_widget_distance distance,vsx_widget_coords coords);
  bool event_key_down(signed long key, bool alt = false, bool ctrl = false, bool shift = false);
  void event_mouse_double_click(vsx_widget_distance distance,vsx_widget_coords coords,int button);
  void event_mouse_wheel(float y);
  void set_filter_string(vsx_string &filter_string);
  void fold_all();
};

class vsx_widget_base_editor : public vsx_widget_panel {
public:
  vsx_widget_scrollbar* scrollbar_horiz;
  vsx_widget_scrollbar* scrollbar_vert;
  vsx_widget_base_edit* editor;

  vsx_widget_base_editor();
  void set_string(const vsx_string& str);
  vsx_string get_string();
  virtual void i_draw();
};

#endif
