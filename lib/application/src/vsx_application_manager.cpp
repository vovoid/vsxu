#include <vsx_application_manager.h>


vsx_application_manager* vsx_application_manager::get_instance()
{
  static vsx_application_manager vam;
  return &vam;
}
