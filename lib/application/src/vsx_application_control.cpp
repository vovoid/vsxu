#include <vsx_application_control.h>





vsx_application_control* vsx_application_control::get_instance()
{
  static vsx_application_control vac;
  return &vac;
}
