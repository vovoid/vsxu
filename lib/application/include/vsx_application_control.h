#pragma once

#include <string/vsx_string.h>
#include "vsx_application_input_mouse_control.h"

class vsx_application_control
{
  bool shutdown = false;
  bool create_preferences_path = false;
  bool show_console_window = false;

public:

  // mouse control
  vsx_application_mouse_control mouse;

  // preferences path
  vsx_string<> preferences_path;

  // message box
  vsx_string<> message_box_title;
  vsx_string<> message_box_message;

  // window title
  vsx_string<> window_title;

  void create_preferences_path_request()
  {
    create_preferences_path = true;
  }

  bool is_preferences_path_creation_requested()
  {
    return create_preferences_path;
  }

  // shut down the application
  void shutdown_request()
  {
    shutdown = true;
  }

  bool is_shutdown_requested()
  {
    return shutdown;
  }

  bool is_show_console_window_requested()
  {
    return show_console_window;
  }

  void show_console_window_request()
  {
    show_console_window = true;
  }

  void show_console_window_reset()
  {
    show_console_window = false;
  }

  static vsx_application_control* get_instance();
};

#define vsx_mouse_control vsx_application_control::get_instance()->mouse
