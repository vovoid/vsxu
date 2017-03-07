#pragma once

#include <vsx_module_list_abs.h>
#include <tools/vsx_singleton.h>

class vsx_module_list_manager
    : public vsx::singleton<vsx_module_list_manager>
{
public:
  vsx_module_list_abs* module_list = 0x0;
};
