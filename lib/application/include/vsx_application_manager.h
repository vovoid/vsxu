#pragma once

#include "vsx_application.h"

class vsx_application_manager
{
  vsx_application* application_ptr;
public:

  void set(vsx_application* application)
  {
    application_ptr = application;
  }

  vsx_application* get()
  {
    return application_ptr;
  }

  static vsx_application_manager* get_instance()
  {
    static vsx_application_manager vam;
    return &vam;
  }
};
