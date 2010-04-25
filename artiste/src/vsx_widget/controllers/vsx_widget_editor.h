#ifndef VSX_NO_CLIENT
#ifndef VSX_WIDGET_EDITOR_H
#define VSX_WIDGET_EDITOR_H


class vsx_widget_editor : public vsx_widget_base_controller {
  
  vsx_widget* implement_button;
  vsx_widget* menu_button;
  vsx_vector pos_, size_;
  
  int longest_line;
  int characters_width;
  int characters_height;
  
  //vsx_widget* scrollbar_horiz;
  //vsx_widget* scrollbar_vertical;
  bool no_move; // if user clicked in the text or not
  
  void save();
  
  vsx_widget* panel;
  vsx_widget* editor;
  vsx_widget* e_log;

public:
  vsx_widget_editor();
  void i_draw();

  vsx_string return_command; // will return a command like [return_command] [base64-encoded-text]
  vsx_string target_param;
  vsx_widget* return_component;
  int scroll_x, scroll_y;
  // vsx widget methods
  void init();
  
  virtual void vsx_command_process_b(vsx_command_s *t);
  
  // custom methods
  bool load_text(vsx_string new_text);
};  

//-------------------------------------------------------------------------




#endif
#endif
