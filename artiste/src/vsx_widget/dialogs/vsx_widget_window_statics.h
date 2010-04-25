#ifndef VSX_NO_CLIENT
#ifndef VSX_WINDOW_STATICS_H
#define VSX_WINDOW_STATICS_H

#include "window/vsx_widget_window.h"

class dialog_query_string : public vsx_widget_window {
  vsx_string i_hint;
  //std::map<vsx_string, vsx_widget*> fields;
  vsx_avector<vsx_widget*> edits;
public:
  vsx_string extra_value; // appended at the end of the command
  vsx_widget *edit1;
  void vsx_command_process_b(vsx_command_s *t);
  
  void show(vsx_string value);
  void show();
  void init() {};
  void set_allowed_chars(vsx_string ch);
  dialog_query_string(vsx_string title_, vsx_string in_fields = "");
};

//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------

class dialog_messagebox : public vsx_widget_window {
public:
  bool allow_resize_y;

  void vsx_command_process_b(vsx_command_s *t) {
    if (t->cmd == "ok") {
      k_focus = parent;
      a_focus = parent;
      _delete();
    }
    visible = 0;
  }
  void init() {
    if (!init_run) vsx_widget_window::init();
  }
  
  virtual void init_children() {
  }
  
  void show(vsx_string value) {
    show();
  }
  

  virtual bool event_key_down(signed long key, bool alt, bool ctrl, bool shift) {
    k_focus = parent;
    a_focus = parent;
    _delete();
    return true;
  }

  void show() {
    visible = 1;
    pos.x = 0.5-size.x/2;
    pos.y = 0.5-size.y/2;
  }

  dialog_messagebox(vsx_string title_,vsx_string hint);
};

#endif
#endif
