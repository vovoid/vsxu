#include <map>
#include <vector>
#include <vsx_string.h>
#include <vsx_param.h>
#include <vsx_module.h>

#include "vsx_module_list_abs.h"
#include "vsx_module_list.h"
#include "vsx_module_list_factory.h"

vsx_module_list_abs* vsx_module_list_factory_create(vsx_string args)
{
    vsx_module_list* module_list = new vsx_module_list();
    module_list->init(args);
    return (vsx_module_list_abs*)module_list;
}

void vsx_module_list_factory_destroy( vsx_module_list_abs* object )
{
  object->destroy();
  delete (vsx_module_list*)object;
}


