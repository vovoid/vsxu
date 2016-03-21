#pragma once

#include "vsx_application_input_mouse_control.h"

class vsx_application_control
{
  bool shutdown = false;
public:

  vsx_application_mouse_control mouse;

  void shutdown_request()
  {
    shutdown = true;
  }

  bool is_shutdown_requested()
  {
    return shutdown;
  }

  static vsx_application_control* get_instance();
};

#define vsx_mouse_control vsx_application_control::get_instance()->mouse
