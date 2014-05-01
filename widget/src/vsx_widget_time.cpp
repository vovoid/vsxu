
#include "vsx_widget_time.h"

vsx_widget_time* vsx_widget_time::get_instance()
{
  static vsx_widget_time t;
  return &t;
}
