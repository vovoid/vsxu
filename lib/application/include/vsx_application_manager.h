#pragma once

#include "vsx_application.h"

class vsx_application_manager
{
  vsx_application* application_ptr;

public:

  void application_set(vsx_application* application)
  {
    application_ptr = application;
  }

  vsx_application* application_get()
  {
    return application_ptr;
  }

  static vsx_application* get()
  {
    return vsx_application_manager::get_instance()->application_get();
  }

  static vsx_application_manager* get_instance();
};
