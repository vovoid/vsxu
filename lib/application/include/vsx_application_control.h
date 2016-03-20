#pragma once


class vsx_application_control
{
  bool shutdown = false;
public:

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
